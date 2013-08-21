/***************************************************************************
 *   OSHDLC - Open Source HDLC Transceiver                                 *
 *                                                                         *
 *   hdlcrec.c - HDLC Receiver performs, flag detection, zero-bit deletion *
 *               and CRC check.                                            *
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

#include "hdlcrec.h"

#include <stdio.h>
#include <stdlib.h>

typedef void ( *hdlcrec_fxn_t ) ( register hdlcrec_t * );

static void hdlcrec_fxn_idle ( register hdlcrec_t * );
static void hdlcrec_fxn_abrt ( register hdlcrec_t * );
static void hdlcrec_fxn_berr ( register hdlcrec_t * );
static void hdlcrec_fxn_ins3 ( register hdlcrec_t * );
static void hdlcrec_fxn_ins4 ( register hdlcrec_t * );
static void hdlcrec_fxn_bop0 ( register hdlcrec_t * );
static void hdlcrec_fxn_bop1 ( register hdlcrec_t * );
static void hdlcrec_fxn_bop2 ( register hdlcrec_t * );
static void hdlcrec_fxn_bop3 ( register hdlcrec_t * );

#define hdlcrec_fxn_ins( hr, bc ) \
{ \
	(hr)->bitbuf |= ( ( (hr)->o_state.field.bits.nibble & ~( 0x00ffu << (bc) ) ) << (hr)->bitcnt );\
   (hr)->bitcnt += (bc);\
}

#define hdlcr_reset_bitbuf( hr ) \
{ \
	(hr)->bitbuf = (hr)->bitcnt = 0; \
}

#define hdlcr_close_packet( hr, msk, np ) \
{ \
	if ( (hr)->octet_count ) \
	{ \
		hdlc_buf_t *buf = (hr)->out_buf; \
		\
		(hr)->octet_count = 0; \
		\
		buf->data [ buf->wi ] &= 0x00ffu; \
		buf->data [ buf->wi ] |= (msk); \
		\
		buf->wi = ( buf->wi + 1 ) & buf->cap_mask; \
		buf->size++; \
		\
		(hr)->flags.bits.new_packet = np; \
	} \
}

#ifdef __CRC16__
	#define hdlcr_good_packet( hr ) \
	{ \
		(hr)->flags.bits.flag_det = TRUE; \
		\
		if ( (hr)->crc16 == CRC_CCITT_CHECKVAL ) { \
			hdlcr_close_packet ( hr, HDLCREC_MASK_LASTBYTE, TRUE ); \
		} \
		else { \
			hdlcr_close_packet ( hr, HDLCREC_MASK_CRCERROR, FALSE ); \
		} \
	}
#else
	#define hdlcr_good_packet( hr ) \
	{ \
		(hr)->flags.bits.flag_det = TRUE; \
		hdlcr_close_packet ( hr, HDLCREC_MASK_LASTBYTE, FALSE ); \
	}
#endif /*__CRC16__*/

#define hdlcr_abort_packet( hr ) \
{ \
	(hr)->flags.bits.abort_det = TRUE; \
	hdlcr_close_packet ( hr, HDLCREC_MASK_ABORT, FALSE ); \
}

#define hdlcr_error_packet( hr ) \
{ \
	(hr)->flags.bits.biterr_det = TRUE; \
	hdlcr_close_packet ( hr, HDLCREC_MASK_BITERROR, FALSE ); \
}


/*
 *	ATTENTION: the order must match to the hdlcrec_cmd_t enum list!
 */

static const hdlcrec_fxn_t hdlcrec_fxn [] = {
			                                    hdlcrec_fxn_idle,
			                                    hdlcrec_fxn_abrt,
			                                    hdlcrec_fxn_berr,
			                                    hdlcrec_fxn_ins3,
			                                    hdlcrec_fxn_ins4,
			                                    hdlcrec_fxn_bop0,
			                                    hdlcrec_fxn_bop1,
			                                    hdlcrec_fxn_bop2,
			                                    hdlcrec_fxn_bop3
			                                    };

static const UINT16 hdlcrec_table [] = {
                                         #include "hdlcr_table.h"
                                         };


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

static void hdlcrec_proc_bitbuf ( hdlcrec_t *hdlcr )
{
	if ( hdlcr->bitcnt >= 8 )
	{
		UINT8 octet = LOBYTE ( hdlcr->bitbuf );
		hdlc_buf_t *buf = hdlcr->out_buf;

		//
		// Check for buffer capacity!

		if ( hdlcr->octet_count >= hdlcr->max_packet_len )
		{
			hdlcr_reset_bitbuf ( hdlcr );
			hdlcr_error_packet ( hdlcr );
			return;
		}

		if ( hdlcr->octet_count++ )
		{
			buf->wi = ( buf->wi + 1 ) & buf->cap_mask;
			buf->size++;

			buf->data [ buf->wi ] = HDLCREC_MASK_MIDDLEBYTE | octet;
		}
		else //first octet received
		{
			buf->data [ buf->wi ] = HDLCREC_MASK_FIRSTBYTE | octet;

		#ifdef __CRC16__
			hdlcr->crc16 = CRC_CCITT_INITIAL;
		#endif
		}

	#ifdef __CRC16__
		hdlcr->crc16 = update_crc_ccitt ( hdlcr->crc16, octet );
	#endif

		hdlcr->bitbuf >>= 8;
      hdlcr->bitcnt -= 8;
	}
}
//---------------------------------------------------------------------------

/*
 * HDLC Receiver, "process octet" function:
 */

void hdlcrec_proc_octet ( hdlcrec_t *hdlcr, UINT8 i_octet )
{
	//
	// process low-nibble first:

	hdlcr->i_state.field.bits.nibble = LONIBBLE ( i_octet );
   hdlcr->o_state.value = hdlcrec_table [ hdlcr->i_state.value ];
   hdlcrec_fxn [ hdlcr->o_state.field.bits.command ] ( hdlcr );
   hdlcr->i_state.field.state.state = hdlcr->o_state.field.state.state;

   hdlcrec_proc_bitbuf ( hdlcr );

   //
	// then, process high-nibble:

   hdlcr->i_state.field.bits.nibble = HINIBBLE ( i_octet );
   hdlcr->o_state.value = hdlcrec_table [ hdlcr->i_state.value ];
   hdlcrec_fxn [ hdlcr->o_state.field.bits.command ] ( hdlcr );
   hdlcr->i_state.field.state.state = hdlcr->o_state.field.state.state;

	hdlcrec_proc_bitbuf ( hdlcr );
}
//---------------------------------------------------------------------------

void hdlcrec_fxn_idle ( register hdlcrec_t *hdlcr )
{
   //do nothing
#ifdef HDLC_DEBUG
	//printf ( "I" );
	//fflush ( stdout );
#endif
}
//---------------------------------------------------------------------------

void hdlcrec_fxn_abrt ( register hdlcrec_t *hdlcr )
{
#ifdef HDLC_DEBUG
	printf ( "A" );
   fflush ( stdout );
#endif

	hdlcr_reset_bitbuf ( hdlcr );
	hdlcr_abort_packet ( hdlcr );
}
//---------------------------------------------------------------------------

void hdlcrec_fxn_berr ( register hdlcrec_t *hdlcr )
{
#ifdef HDLC_DEBUG
   printf ( "E" );
	fflush ( stdout );
#endif

	hdlcr_reset_bitbuf ( hdlcr );
	hdlcr_error_packet ( hdlcr );
}
//---------------------------------------------------------------------------

void hdlcrec_fxn_ins3 ( register hdlcrec_t *hdlcr )
{
   hdlcrec_fxn_ins ( hdlcr, 3 );
}
//---------------------------------------------------------------------------

void hdlcrec_fxn_ins4 ( register hdlcrec_t *hdlcr )
{
	hdlcrec_fxn_ins ( hdlcr, 4 );
}
//---------------------------------------------------------------------------

void hdlcrec_fxn_bop0 ( register hdlcrec_t *hdlcr )
{
   hdlcr_reset_bitbuf ( hdlcr );
	hdlcr_good_packet ( hdlcr );
}
//---------------------------------------------------------------------------

void hdlcrec_fxn_bop1 ( register hdlcrec_t *hdlcr )
{
   hdlcr_reset_bitbuf ( hdlcr );
	hdlcr_good_packet ( hdlcr );

	hdlcrec_fxn_ins ( hdlcr, 1 );
}
//---------------------------------------------------------------------------

void hdlcrec_fxn_bop2 ( register hdlcrec_t *hdlcr )
{
   hdlcr_reset_bitbuf ( hdlcr );
	hdlcr_good_packet ( hdlcr );

	hdlcrec_fxn_ins ( hdlcr, 2 );
}
//---------------------------------------------------------------------------

void hdlcrec_fxn_bop3 ( register hdlcrec_t *hdlcr )
{
   hdlcr_reset_bitbuf ( hdlcr );
	hdlcr_good_packet ( hdlcr );

	hdlcrec_fxn_ins ( hdlcr, 3 );
}
//---------------------------------------------------------------------------


