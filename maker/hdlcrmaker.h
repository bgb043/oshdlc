/***************************************************************************
 *   OSHDLC - Open Source HDLC Transceiver                                 *
 *                                                                         *
 *   hdlcrmaker.c - HDLC Receiver Table Maker creates the receiver table   *
 *                  and writes it down to a text file.                     *
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

#ifndef __HDLCRMAKER_H
#define __HDLCRMAKER_H

#include "version.h"
#include "types.h"

/*
   HDLC RECEIVER BASICS:
   =====================

   Flags:
   ------
      Every HDLC packet must begin with at least one openning flag (01111110)
      and an opening flag indicates the end of the previous (if exists) packet as well.

   Zero Bit Deletion:
   ------------------
      Because of the nature of the openning flag, after a valid opening flag
      detection, reception of a '0' bit just after 5 consequetive '1's must
      be deleted!
*/

typedef enum {
               HDLCR_CMD_IDLE, // idle command, no action!
               HDLCR_CMD_ABRT, // hdlc abort
               HDLCR_CMD_BERR, // hdlc bit error detected
               HDLCR_CMD_INS3, // insert the 3 bits extracted
               HDLCR_CMD_INS4, // insert the 4 bits extracted
               HDLCR_CMD_BOP0, // begining of packet preceeded by no extracted bits
               HDLCR_CMD_BOP1, // begining of packet preceeded by 1 extracted bits
               HDLCR_CMD_BOP2, // begining of packet preceeded by 2 extracted bits
               HDLCR_CMD_BOP3, // begining of packet preceeded by 3 extracted bits

               HDLCR_CMD_INVALID,  //invalid command!!!!

               HDLCR_NUM_CMDS  // total number of commands defined
               } hdlcr_cmd_t;

#define HDLCR_NBITS_ONESCNT   3
#define HDLCR_NBITS_OPENFLAG  1
#define HDLCR_NBITS_ZEROFLAG  1
#define HDLCR_NBITS_NIBBLE    4
#define HDLCR_NBITS_CMD       7

#define HDLCR_NBITS_STATE     ( HDLCR_NBITS_ONESCNT + HDLCR_NBITS_OPENFLAG + HDLCR_NBITS_ZEROFLAG )

typedef union __attribute__ ((__packed__))
{
   union
   {
   #if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)

      struct __attribute__ ((__packed__))
      {
         unsigned ones_cnt    : HDLCR_NBITS_ONESCNT;    // # of '1's counted
         unsigned open_flag   : HDLCR_NBITS_OPENFLAG;   // opening flag "01111110" detected flag
         unsigned zero_flag   : HDLCR_NBITS_ZEROFLAG;   // at least one '0' bit received flag
         unsigned nibble      : HDLCR_NBITS_NIBBLE;     // input nibble/extracted bits
         unsigned command     : HDLCR_NBITS_CMD;        // output command (HDLCR_CMD_xxx)

      } bits;

      struct __attribute__ ((__packed__))
      {
         unsigned state  : HDLCR_NBITS_STATE;
         unsigned unused : 16 - HDLCR_NBITS_STATE;

      } state;

   #else /* Big-Endian byte order */

      struct __attribute__ ((__packed__))
      {
         unsigned unused : 16 - HDLCR_NBITS_STATE;
         unsigned state  : HDLCR_NBITS_STATE;
      } state;

      struct __attribute__ ((__packed__))
      {
         unsigned command     : HDLCR_NBITS_CMD;        // output command (HDLCR_CMD_xxx)
         unsigned nibble      : HDLCR_NBITS_NIBBLE;     // input nibble/extracted bits
         unsigned zero_flag   : HDLCR_NBITS_ZEROFLAG;   // at least one '0' bit received flag
         unsigned open_flag   : HDLCR_NBITS_OPENFLAG;   // opening flag "01111110" detected flag
         unsigned ones_cnt    : HDLCR_NBITS_ONESCNT;    // # of '1's counted
      } bits;

   #endif /*__BYTE_ORDER__*/

   } field;

   uint16_t value;

} hdlcr_state_t;

extern int hdlcr_maker ( char out_file_name[], BOOL comments_on );

#endif /*__HDLCRMAKER_H*/
