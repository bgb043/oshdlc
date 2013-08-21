/***************************************************************************
 *   OSHDLC - Open Source HDLC Transceiver                                 *
 *                                                                         *
 *   hdlctrans.c - HDLC Transmitter performs, flag encapsulation, zero-bit *
 *                 and CRC insertion.                                      *
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

#include "hdlctrans.h"
#include <string.h>

extern void save_tim ( int );

static const UINT16 hdlct_state_table [] = {
															#include "hdlct_table.h"
															};


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void hdlctrans_proc_octet (
										hdlctrans_t	*ht,
										const UINT8	i_octet
										)
{
	hdlct_output_state_t o_state;
	UINT32 bitbuf = ht->bitbuf;
	INT8 bitcnt = ht->bitcnt;
	hdlc_buf_t *buf = ht->out_buf;

#ifdef HDLC_DEBUG
	//printf ("%02x ", i_octet);
#endif

	//
	// transmit low-nibble:

	ht->i_state.field.nibble = LONIBBLE ( i_octet );

	o_state.value = hdlct_state_table [ ht->i_state.value ];
	ht->i_state.field.ones_cnt = o_state.field.ones_cnt; //update state

	bitbuf |= ( (UINT32) o_state.field.bits << bitcnt );
	bitcnt += ( o_state.field.zbi_flag ? 5 : 4 );

	//
	// transmit high-nibble:

	ht->i_state.field.nibble = HINIBBLE ( i_octet );

	o_state.value = hdlct_state_table [ ht->i_state.value ];
	ht->i_state.field.ones_cnt = o_state.field.ones_cnt; //update state

	bitbuf |= ( (UINT32) o_state.field.bits << bitcnt );
	bitcnt += ( o_state.field.zbi_flag ? 5 : 4 );

	while ( bitcnt >= 8 )
	{
		hdlc_buf_put ( buf, LOBYTE ( bitbuf ) );

      bitbuf >>= 8;
		bitcnt -= 8;
	}

	ht->bitbuf = bitbuf;
	ht->bitcnt = bitcnt;
}
//--------------------------------------------------------------------------

