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

#ifndef __HDLCREC_H
#define __HDLCREC_H

#include "hdlcbuf.h"
#ifdef __CRC16__
	#include "lib_crc.h"
#endif

#define HDLCR_NBITS_ONESCNT   3
#define HDLCR_NBITS_OPENFLAG  1
#define HDLCR_NBITS_ZEROFLAG  1
#define HDLCR_NBITS_NIBBLE    4
#define HDLCR_NBITS_CMD       7

#define HDLCR_NBITS_STATE     ( HDLCR_NBITS_ONESCNT + HDLCR_NBITS_OPENFLAG + HDLCR_NBITS_ZEROFLAG )

typedef union _hdlcr_state_t
{
   union
   {
      struct
      {
			unsigned state  : HDLCR_NBITS_STATE;
         unsigned unused : 16 - HDLCR_NBITS_STATE;
      } state;

      struct
      {
      	unsigned ones_cnt    : HDLCR_NBITS_ONESCNT;    // # of '1's counted
      	unsigned open_flag   : HDLCR_NBITS_OPENFLAG;   // opening flag "01111110" detected flag
         unsigned zero_flag   : HDLCR_NBITS_ZEROFLAG;   // at least one '0' bit received flag
         unsigned nibble      : HDLCR_NBITS_NIBBLE;     // input nibble/extracted bits
         unsigned command     : HDLCR_NBITS_CMD;        // output command (HDLCR_CMD_xxx)
      } bits;
   } field;

   unsigned short value;

} hdlcr_state_t;


typedef struct hdlcrec_t
{
	union
	{
		struct
		{
			unsigned biterr_det : 1;	/*bit error detected*/
			unsigned abort_det  : 1;	/*abort detected*/
			unsigned flag_det   : 1;	/*open/close flag detected*/
			unsigned new_packet : 1;	/*new packet received*/
		} bits;

		unsigned short val;

	} flags;

	hdlcr_state_t i_state, o_state;
   UINT16 bitbuf;
   INT8 bitcnt;
	UINT16 octet_count;
	UINT16 max_packet_len;

#ifdef __CRC16__
	UINT16 crc16;
#endif

   hdlc_buf_t *out_buf;

} hdlcrec_t;


	/*
	 * HDLC Receiver Queue Octet Masks:
	 */
#define HDLCREC_MASK_FIRSTBYTE		0x0100	/* first byte of packet */
#define HDLCREC_MASK_MIDDLEBYTE		0x0200	/* middle byte of packet */
#define HDLCREC_MASK_LASTBYTE			0x0400	/* last byte of good packet (correct FCS) */
#define HDLCREC_MASK_BITERROR			0x0800	/* bit error */
#ifdef __CRC16__
	#define HDLCREC_MASK_CRCERROR		0x1000	/* CRC check error */
#endif
#define HDLCREC_MASK_ABORT				0x2000	/* HDLC aborted! */

	/*
	 * HDLC Receiver API/Macro:
	 */

#define hdlcrec_init( hr, v_buf, maxpl ) \
{ \
	memset ( &(hr), 0, sizeof ( hdlcrec_t ) ); \
	hdlc_buf_init ( v_buf ); \
	(hr).out_buf = &(v_buf); \
	(hr).max_packet_len = maxpl; \
}

extern void hdlcrec_proc_octet ( hdlcrec_t *, UINT8 );


#endif /*__HDLCREC_H*/
