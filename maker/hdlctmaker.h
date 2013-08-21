/***************************************************************************
 *   OSHDLC - Open Source HDLC Transceiver                                 *
 *                                                                         *
 *   hdlctmaker.c - HDLC Transmitter Table Maker creates the transmitter   *
 *                  table and writes it down to a text file.               *
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

#ifndef __HDLCTMAKER_H
#define __HDLCTMAKER_H

#include "version.h"
#include "types.h"

#define HDLC_OPENING_FLAG		0x7e					/*HDLC opening flag*/
#define HDLC_CLOSING_FLAG		HDLC_OPENING_FLAG	/*HDLC closing flag*/
#define HDLC_ABORT_PATRN		0xff					/*at least 7 ones needed for abort!*/

#define HDLCT_NBITS_ONESCNT    3  /*ones' count*/
#define HDLCT_NBITS_TRANSFLAG  1  /*transparent mode flag*/
#define HDLCT_NBITS_NIBBLE     4  /*number of input bits*/
#define HDLCT_NBITS_GBITS      5  /*num of generated bits including zero ins. */
#define HDLCT_NBITS_ZBIF       1  /*zero bit insertion flag*/

#define HDLCT_NBITS_ISTATE  ( HDLCT_NBITS_ONESCNT + HDLCT_NBITS_TRANSFLAG + HDLCT_NBITS_NIBBLE )

#define HDLCT_ISTATE_MAXVAL  ( 1 << HDLCT_NBITS_ISTATE )

typedef union __attribute__ ((__packed__))
{
   struct __attribute__ ((__packed__))
   {
   #if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)

      unsigned ones_cnt    : HDLCT_NBITS_ONESCNT;
      unsigned trans_flag  : HDLCT_NBITS_TRANSFLAG;
      unsigned nibble      : HDLCT_NBITS_NIBBLE;

      unsigned dummy			: ( 16 -
   									 HDLCT_NBITS_NIBBLE    -
   									 HDLCT_NBITS_TRANSFLAG -
   									 HDLCT_NBITS_ONESCNT
   									 );

   #else /* Big-Endian byte order */
      unsigned dummy			: ( 16 -
   									 HDLCT_NBITS_NIBBLE    -
   									 HDLCT_NBITS_TRANSFLAG -
   									 HDLCT_NBITS_ONESCNT
   									 );

      unsigned nibble      : HDLCT_NBITS_NIBBLE;
      unsigned trans_flag  : HDLCT_NBITS_TRANSFLAG;
      unsigned ones_cnt    : HDLCT_NBITS_ONESCNT;

   #endif /*__BYTE_ORDER__*/

   } field;

   uint16_t value;

} hdlct_input_state_t;

typedef union __attribute__ ((__packed__))
{
   struct __attribute__ ((__packed__))
   {
   #if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)

      unsigned ones_cnt : HDLCT_NBITS_ONESCNT;
      unsigned zbi_flag : HDLCT_NBITS_ZBIF;
      unsigned bits     : HDLCT_NBITS_GBITS;

      unsigned dummy		: ( 16 -
   								 HDLCT_NBITS_GBITS   -
   								 HDLCT_NBITS_ZBIF 	-
   								 HDLCT_NBITS_ONESCNT
   								 );

   #else /* Big-Endian byte order */

   	unsigned dummy		: ( 16 -
   								 HDLCT_NBITS_GBITS   -
   								 HDLCT_NBITS_ZBIF 	-
   								 HDLCT_NBITS_ONESCNT
   								 );

      unsigned bits     : HDLCT_NBITS_GBITS;
      unsigned zbi_flag : HDLCT_NBITS_ZBIF;
      unsigned ones_cnt : HDLCT_NBITS_ONESCNT;

   #endif /*__BYTE_ORDER__*/

   } field;

   uint16_t value;

} hdlct_output_state_t;

extern int hdlct_maker ( char out_file_name[], BOOL comments_on );

#endif /*__HDLCTMAKER_H*/
