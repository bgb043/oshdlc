/***************************************************************************
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

#ifndef __HDLCTRANS_H
#define __HDLCTRANS_H

#include "hdlcbuf.h"
#ifdef __CRC16__
	#include "lib_crc.h"
#endif

#define HDLC_OPENING_FLAG		0x7e
#define HDLC_CLOSING_FLAG		0x7e
#define HDLC_ABORT_PATRN		0xff	/*at least 7 ones needed for abort!*/

#define HDLCT_NBITS_ONESCNT    3  /*ones' count*/
#define HDLCT_NBITS_TRANSFLAG  1  /*transparent mode flag*/
#define HDLCT_NBITS_NIBBLE     4  /*number of input bits*/
#define HDLCT_NBITS_GBITS      5  /*num of generated bits including zero ins. */
#define HDLCT_NBITS_ZBIF       1  /*zero bit insertion flag*/

#define HDLCT_NBITS_ISTATE  ( HDLCT_NBITS_ONESCNT + HDLCT_NBITS_TRANSFLAG + HDLCT_NBITS_NIBBLE )

#define HDLCT_ISTATE_MAXVAL  ( 1 << HDLCT_NBITS_ISTATE )

typedef union
{
   struct
   {
		unsigned ones_cnt    : HDLCT_NBITS_ONESCNT;
		unsigned trans_flag  : HDLCT_NBITS_TRANSFLAG;
		unsigned nibble      : HDLCT_NBITS_NIBBLE;

   	unsigned dummy			: ( 16 -
   									 HDLCT_NBITS_NIBBLE    -
   									 HDLCT_NBITS_TRANSFLAG -
   									 HDLCT_NBITS_ONESCNT
   									 );
   } field;

   unsigned short value;

} hdlct_input_state_t;

typedef union
{
   struct
   {
		unsigned ones_cnt : HDLCT_NBITS_ONESCNT;
		unsigned zbi_flag : HDLCT_NBITS_ZBIF;
		unsigned bits     : HDLCT_NBITS_GBITS;

   	unsigned dummy		: ( 16 -
   								 HDLCT_NBITS_GBITS   -
   								 HDLCT_NBITS_ZBIF 	-
   								 HDLCT_NBITS_ONESCNT
   								 );
   } field;

   unsigned short value;

} hdlct_output_state_t;

typedef struct hdlctrans_t
{
	hdlct_input_state_t i_state;
	UINT32 bitbuf;
	INT8 bitcnt;

#ifdef __CRC16__
	UINT16 crc16;
#endif

	hdlc_buf_t *out_buf;

} hdlctrans_t;


/*
 * HDLC Transmitter API Functions/Macros (Small memory model!!):
 */

#define hdlctrans_init( ht, v_buf ) \
{ \
	memset ( &(ht), 0, sizeof ( hdlctrans_t ) ); \
	hdlc_buf_init ( v_buf ); \
	(ht).out_buf = &v_buf; \
}

#define hdlctrans_flag( ht ) \
{ \
	hdlctrans_transmod ( ht, TRUE ); \
	hdlctrans_proc_octet ( ht, HDLC_OPENING_FLAG ); \
}

#ifdef __CRC16__
	#define hdlctrans_start( ht ) \
	{ \
		(ht)->crc16 = CRC_CCITT_INITIAL; \
		hdlctrans_flag ( ht ); \
		hdlctrans_transmod ( ht, FALSE ); \
	}
#else
	#define hdlctrans_start( ht ) \
	{ \
		hdlctrans_flag ( ht ); \
	}
#endif

extern void hdlctrans_proc_octet ( hdlctrans_t *, const UINT8 );

#ifdef __CRC16__
	#define hdlctrans_packetbyte( ht, byte ) \
	{ \
		hdlctrans_proc_octet ( ht, byte ); \
		(ht)->crc16 = update_crc_ccitt ( (ht)->crc16, byte ); \
	}
#else
	#define hdlctrans_packetbyte( ht, byte ) \
	{ \
		hdlctrans_proc_octet ( ht, byte ); \
	}
#endif

#ifdef __CRC16__
	#define hdlctrans_end( ht ) \
	{ \
		(ht)->crc16 = ~(ht)->crc16; \
		 hdlctrans_proc_octet ( ht, HIBYTE( (ht)->crc16 ) ); \
	 	 hdlctrans_proc_octet ( ht, LOBYTE( (ht)->crc16 ) ); \
	 	 hdlctrans_flag ( ht ); \
	 	 hdlctrans_flag ( ht ); \
	}
#else
	#define hdlctrans_end( ht ) \
	{ \
		hdlctrans_flag ( ht ); \
		hdlctrans_flag ( ht ); \
	}
#endif

#define hdlctrans_abort( ht ) \
{ \
	hdlctrans_transmod ( ht, TRUE ); \
	hdlctrans_octet ( ht, HDLC_ABORT_PATRN ); \
}

#define hdlctrans_transmod( ht, s ) \
{ \
	(ht)->i_state.field.trans_flag = s; \
}

#endif /*__HDLCTRANS_H*/
