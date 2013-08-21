/***************************************************************************
 *   OSHDLC - Open Source HDLC Transceiver                                 *
 *                                                                         *
 *   hdlclib.c - HDLC channel management routines.                         *
 *                                                                         *
 *                                                                         *
 *   Copyright (C) 2008 DIPCENTER, DIPMAN                                  *
 *   dipcenter@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "hdlclib.h"
#include <stdlib.h>

//---------------------------------------------------------------------

/* Free up all the packet queue of the HDLC channel.
 */
static void HDLC_freePacketQueue ( hdlc_chan_t *p_hdlc )
{
	while ( !DQUE_IsEmpty ( (DQUE_Handle) &p_hdlc->rec_buf ) )
	{
		hdlc_packet_t *ptr = (hdlc_packet_t *) p_hdlc->rec_buf.qelem.next;

		DQUE_Del ( (DQUE_Elem *) ptr );
		free ( ptr );
	}
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------

/* HDLC Create Channel:
 *    Creates an independent HDLC channel and returns an HDLC handle for use.
 *    On error, returns NULL.
 */

hdlc_chan_t *HDLC_createChannel ( void )
{
	hdlc_chan_t *p_hdlc = (hdlc_chan_t *) malloc ( sizeof ( hdlc_chan_t ) );

	if ( !p_hdlc )
		return NULL; /*FAILS!!*/

	DQUE_New ( (DQUE_Handle) &p_hdlc->rec_buf );
	p_hdlc->r_pi = 0;

	hdlctrans_init ( p_hdlc->hdlct, p_hdlc->hdlct_obuf );
	hdlcrec_init ( p_hdlc->hdlcr, p_hdlc->hdlcr_obuf, HDLC_PKT_MAXLEN );

	return p_hdlc;
}
//---------------------------------------------------------------------

/* Destroy (free-up) the created HDLC channel.
 */
void HDLC_destroyChannel ( hdlc_chan_t **p_hdlc )
{
	if ( *p_hdlc )
	{
		HDLC_freePacketQueue ( *p_hdlc );
		free ( *p_hdlc );
		*p_hdlc = NULL;
	}
}
//---------------------------------------------------------------------

/* Reset the HDLC channel.
 */
void HDLC_resetChannel ( hdlc_chan_t *p_hdlc )
{
	/*reinitialize the tranmitter and receiver*/
	hdlctrans_init ( p_hdlc->hdlct, p_hdlc->hdlct_obuf );
	hdlcrec_init ( p_hdlc->hdlcr, p_hdlc->hdlcr_obuf, HDLC_PKT_MAXLEN );

	/*clean up all the preallocated packets:*/
	HDLC_freePacketQueue ( p_hdlc );
	p_hdlc->r_pi = 0;
}
//---------------------------------------------------------------------

/* HDLC Packet Encoder Function :
 *
 * Input Parameters:
 * =================
 * p_hdlc  : HDLC codec handle
 * inp     : Input packet data
 * i_size  : size of the input packet (# of octets)
 * out     : HDLC encoded data (min alloc size must be HDLC_PKT_MAXLEN)
 *           { = open flag(s) + zero-bit inserted data + CRC16 + closing flag(s) }
 * o_cap   : Output buffer capacity. It must be >= ( i_size + 4 ) * 1.2
 *
 * Returns:
 * ========
 * Negative number on error else # of output octets.
 */

int HDLC_encode ( hdlc_chan_t *p_hdlc, const UINT8 inp[ ], int i_size, UINT8 out[ ], int o_cap )
{
	hdlctrans_t *hdlct = &p_hdlc->hdlct;
	int i;

	if ( i_size > HDLC_PKT_MAXLEN )
	{
		return -1; //ERROR: packet too large!!!
	}

	hdlctrans_start ( hdlct );

	for ( i = 0; i < i_size; i++ )
	{
		hdlctrans_packetbyte ( hdlct, inp[ i ] );
	}

	hdlctrans_end ( hdlct );

	i = 0;

	while ( !hdlc_buf_empty ( hdlct->out_buf ) )
	{
		if ( i < o_cap )
			out[ i++ ] = hdlc_buf_get ( hdlct->out_buf );
		else
			return -2; //ERROR: output buffer capacity overflow!
	}

	return i;
}
//---------------------------------------------------------------------

/* HDLC Packet Decoder Function :
 *
 * Input Parameters:
 * =================
 * p_hdlc  : HDLC codec handle
 * inp     : Input packet data { = open flag(s) + zero-bit inserted data + CRC16 + closing flag(s) }
 * i_size  : size of the input packet (# of octets)
 *
 * Return :
 * ========
 * # of packets extracted. If no packetrs detected then 0 returns.
 */

int HDLC_decode ( hdlc_chan_t *p_hdlc, const UINT8 inp[ ], int i_size )
{
	int retval = 0;
	hdlcrec_t *hdlcr = &p_hdlc->hdlcr;
	hdlc_buf_t *hdlcr_obuf;

	hdlcr_obuf = hdlcr->out_buf;

	while ( i_size-- )
	{
		hdlcr->flags.val = 0; //zero-out all HDLC flags (user must clear these flags!)
		hdlcrec_proc_octet ( hdlcr, *inp++ ); //process the next octet

		while ( !hdlc_buf_empty ( hdlcr_obuf ) )
		{
			UINT16 data = hdlc_buf_get ( hdlcr_obuf );

			if ( data & HDLCREC_MASK_MIDDLEBYTE )
			{
				if ( p_hdlc->r_pi >= 1 && p_hdlc->r_pi < HDLC_PKT_MAXLEN - 1 )
				{
					p_hdlc->r_packet [ p_hdlc->r_pi++ ] = LOBYTE ( data );
				}
				else
					p_hdlc->r_pi = 0;
			}
			else if ( data & HDLCREC_MASK_FIRSTBYTE )
			{
				p_hdlc->r_packet [ 0 ] = LOBYTE ( data );
				p_hdlc->r_pi = 1;
			}
			else if ( data & HDLCREC_MASK_LASTBYTE )
			{
				if ( p_hdlc->r_pi >= 1 && p_hdlc->r_pi < HDLC_PKT_MAXLEN - 1 )
				{
					hdlc_packet_t *new_packet;

					p_hdlc->r_packet [ p_hdlc->r_pi++ ] = LOBYTE ( data );

					if ( ( new_packet = (hdlc_packet_t *) malloc ( sizeof ( hdlc_packet_t ) + p_hdlc->r_pi - 1 ) ) != NULL )
					{
						memcpy ( new_packet->data, p_hdlc->r_packet, new_packet->size = p_hdlc->r_pi );
						DQUE_Push ( (DQUE_Handle) &p_hdlc->rec_buf, (DQUE_Elem*) new_packet );
						retval++; //detected one more packet!
					}
				}

				p_hdlc->r_pi = 0;
			}
			else /*if ( data & ( HDLCREC_MASK_BITERROR |
									 HDLCREC_MASK_CRCERROR |
									 HDLCREC_MASK_ABORT ) )*/
			{
				p_hdlc->r_pi = 0;
			}
		}
	}

	return retval;
}
//---------------------------------------------------------------------

/* HDLC_getDecodedPacket :
 *
 * If the function "HDLC_decode" returns a positive integer value then
 * call this function to get the extracted packets.
 *
 * This function returns a pointer to the next extracted packet. If fails
 * it returns just NULL pointer.
 *
 * CARE: Do not forget to free the packet after processing each packet!!!
 */

hdlc_packet_t *HDLC_getDecodedPacket ( hdlc_chan_t *p_hdlc )
{
	return (hdlc_packet_t *) DQUE_Pop ( (DQUE_Handle) &p_hdlc->rec_buf );
}
//---------------------------------------------------------------------

/* Frees the allocated memory for the target packet.
 */

void HDLC_freePacket ( hdlc_packet_t **p_packet )
{
	if ( *p_packet )
	{
		free ( *p_packet );
		*p_packet = NULL;
	}
}
//---------------------------------------------------------------------

