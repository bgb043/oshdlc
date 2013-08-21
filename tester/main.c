/***************************************************************************
 *   OSHDLC - Open Source HDLC Transceiver                                 *
 *                                                                         *
 *   hdlctrans.c - HDLC Tranceiver continuously genrates random packets,   *
 *                 puts them into the HDLC transmitter and loop back       *
 *                 the transmitter output onto the receiver. Finally it    *
 *                 checks for CRC and compares the receiver output with    *
 *                 the original input.                                     *
 *                                                                         *
 *                                                                         *
 *   Copyright (C) 2008 - 2013 DIPCENTER, DIPMAN                           *
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

#include <stdio.h>
#include <stdlib.h>
#include "hdlclib.h"

UINT8 enc_inp[ HDLC_PKT_MAXLEN ],
      enc_out[ HDLCENCSIZE ( HDLC_PKT_MAXLEN ) ],
      *dec_out;

int enc_isize = sizeof ( enc_inp ),
	 enc_osize,
    dec_osize,
    ext_packnum;

hdlc_chan_t *p_hdlc;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

int main ( void )
{
	int i;
	long num_packets = 0;

	printf ( "OSHDLC Tester\n" );

	p_hdlc = HDLC_createChannel ();

	printf ( "HDLC channel handle=0x%p\n", p_hdlc );
	printf ( "Testing " );

	while ( 1 )
	{
		hdlc_packet_t *dec_packet;

		/* Generate random packet:
		 */
		enc_isize = 2 + rand () % 240;

		for ( i = 0; i < enc_isize; i++ )
			enc_inp[ i ] = LOBYTE ( rand() );

		/*encode the packet first*/
		enc_osize = HDLC_encode ( p_hdlc, enc_inp, enc_isize, enc_out, sizeof ( enc_out ) );

		/*then decode the transmitter output*/
		ext_packnum = HDLC_decode ( p_hdlc, enc_out, enc_osize );

		if ( ext_packnum )
		{
			dec_packet = HDLC_getDecodedPacket ( p_hdlc );

			dec_osize = dec_packet->size - 2;  //exclude 2-byte CRC16 octets!
			dec_out = dec_packet->data;

			/*decoded packet size is equal to the original size?*/
			if ( enc_isize != dec_osize )
				goto err;

			/*compare two packets...*/
			for ( i = 0; i < dec_osize; i++ )
			{
				if ( enc_inp[ i ] != dec_out[ i ] )
				{
					HDLC_freePacket ( &dec_packet );
					goto err;
				}
			}

			HDLC_freePacket ( &dec_packet );
		}
		else /*ERROR: no packets extracted!*/
		{
			goto err;
		}

		if ( ( ++num_packets & 0x0003fffL ) == 0 )
			putch ('.');
	}

	return 0;

err:

	printf ("\n!!!ERROR!!!\n");

	printf ("enc_inp[%i] = ", enc_isize);

	for ( i = 0; i < enc_isize; i++ )
		printf ("%02x ", enc_inp[ i ] );

	printf ("\n");

	printf ("enc_out[%i] = ", enc_osize);

	for ( i = 0; i < enc_osize; i++ )
		printf ("%02x ", enc_out[ i ]);

	printf ("\n");

	printf ("dec_out[%i] = ", dec_osize);

	for ( i = 0; i < dec_osize; i++ )
		printf ("%02x ", dec_out[ i ]);

	printf ("\n");

	return -1;
}
//----------------------------------------------------------------------------
