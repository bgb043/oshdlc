/***************************************************************************
 *   OSHDLC - Open Source HDLC Transceiver                                 *
 *                                                                         *
 *   hdlcrmaker.c - HDLC Receiver Table Maker creates the receiver table   *
 *                  and writes it down to a text file.                     *
 *                                                                         *
 *   Copyright (C) 2008 - 2013 DIPCENTER, DIPMAN                                  *
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
#include <string.h>
//#include <time.h>

#include "hdlcrmaker.h"
#include "version.h"

static const char hdlc_cmd_name [ ][ 5 ] = {
                                             "IDLE",
                                             "ABRT",
                                             "BERR",
                                             "INS3",
                                             "INS4",
                                             "BOP0",
                                             "BOP1",
                                             "BOP2",
                                             "BOP3",
                                             "????"  //unknown command
                                             };

static const char hdlc_cmd_help [ ][ 50 ] = {
                                             "Idle",
                                             "Abort",
                                             "Bit Error",
                                             "Insert 3 bits",
                                             "Insert 4 bits",
                                             "Beginning of packet, on insertion",
                                             "Beginning of packet, insert 1 bit",
                                             "Beginning of packet, insert 2 bits",
                                             "Beginning of packet, insert 3 bits",
                                             "Invalid command!!!"
                                             };

#define HDLCR_NBITS_ISTATE  ( HDLCR_NBITS_ONESCNT + HDLCR_NBITS_OPENFLAG + HDLCR_NBITS_ZEROFLAG + HDLCR_NBITS_NIBBLE )

#define HDLCR_ISTATE_MAXVAL  ( 1 << HDLCR_NBITS_ISTATE )  /*3+1+1+4=9 bits*/

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

int hdlcr_maker ( char out_file_name[], BOOL comments_on )
{
	FILE *fp;
	hdlcr_state_t i_state;

   printf ("HDLC Receiver Maker starts...\n");

   if ( ( fp = fopen ( out_file_name, "w" ) ) == NULL )
   {
      printf ("ERROR: unabled to open file \"%s\"\n", out_file_name);
      return  -1;
   }

   fprintf ( fp, "/********************************************************************\\\n" );
   fprintf ( fp, "  HDLC Receiver Maker - Version: %d.%d.%d\n",
                              VERSION_MAJOR, VERSION_MINOR, VERSION_BUILT );

   fprintf ( fp, "  Built @%s-%s\n\n", __DATE__, __TIME__ );

   if ( comments_on )
   {
		int cmd;

      fprintf ( fp, "  Abbreviations:\n" );
      fprintf ( fp, "  --------------\n" );
      fprintf ( fp, "     STx  : input / output state of the HDLC receiver (hex notation)\n" );
      fprintf ( fp, "     Z    : zero flag (%d bits)\n", HDLCR_NBITS_ZEROFLAG );
      fprintf ( fp, "     O    : opening/closing flag (%d bits)\n", HDLCR_NBITS_OPENFLAG );
      fprintf ( fp, "     C    : ones' count (%d bits)\n", HDLCR_NBITS_ONESCNT );
      fprintf ( fp, "     NIBB : input nibble/extracted bits after zero deletion (%d bits)\n", HDLCR_NBITS_NIBBLE );
      fprintf ( fp, "     FUNC : output command function (%d bits)\n", HDLCR_NBITS_CMD );

      for ( cmd = 0; cmd < HDLCR_NUM_CMDS; cmd++ )
      {
         fprintf ( fp, "            %s = %d (%s)\n", hdlc_cmd_name [ cmd ], cmd, hdlc_cmd_help [ cmd ] );
      }

      fprintf ( fp, "     D    : zero bit deletion indicator, not included in output word!(y/n)\n" );
   }

   fprintf ( fp, "\\********************************************************************/\n\n" );

   if ( comments_on )
   {
      fprintf ( fp, "/*  STi: [ NIBB  Z O C ] -->  STo: [ FUNC  NIBB  Z O C ] D  */\n" );
      fprintf ( fp, "/*  ====   ====  = = =        ====   ====  ====  = = =   =  */\n" );
   }


   for ( i_state.value = 0; i_state.value < HDLCR_ISTATE_MAXVAL; i_state.value++ )
   {
      BOOL zero_del = FALSE;
      hdlcr_state_t o_state;
      UINT8 i_nibble = i_state.field.bits.nibble,
            o_msk = 0x01,
            i_msk;
		int num_bits;

      o_state.value = i_state.value;

      o_state.field.bits.nibble = 0x0;
      o_state.field.bits.command = HDLCR_CMD_IDLE;  //for convinience only!

      for ( i_msk = 0x01; i_msk != 0x10; i_msk <<= 1 )
      {
         UINT8 i_bit = i_nibble & i_msk;

         if ( i_bit ) //bit '1' received
         {
            if ( o_state.field.bits.ones_cnt < 7 )
            	o_state.field.bits.ones_cnt++;

            if ( o_state.field.bits.ones_cnt == 7 ) //aborted?
            {
               o_state.field.bits.open_flag = o_state.field.bits.zero_flag = 0;
               o_state.field.bits.ones_cnt = 0;
               o_state.field.bits.command = HDLCR_CMD_ABRT;

               o_msk = 0x01;
            }
            else if ( o_state.field.bits.ones_cnt == 6 ) //shall it be an abort or a flag???
            {
            	o_state.field.bits.nibble = 0x0;
            	o_msk = 0x01;
            }
            else //ones count <= 5
            {
            	if ( o_state.field.bits.open_flag ) //opening flag has been received
               {
             		//
             		// insert the bit '1' into extracted bits:

             		o_state.field.bits.nibble |= o_msk;
             		o_msk <<= 1;
               }
            }
         }
         else //bit '0' received
         {
            if ( o_state.field.bits.ones_cnt == 6 )
            {
            	if ( o_state.field.bits.zero_flag ) //new flag (01111110) detection?
            	{
	               o_state.field.bits.command = HDLCR_CMD_BOP0;
	               o_state.field.bits.open_flag = 1;
	               o_state.field.bits.zero_flag = 0;
               }
               else // invalid flag error: '0111111'
               {
               	if ( o_state.field.bits.open_flag )
               	{
               		o_state.field.bits.command = HDLCR_CMD_BERR;
               		o_state.field.bits.open_flag = 0;
               	}
               	else //waiting for an open flag...
               	{
               		o_state.field.bits.command = HDLCR_CMD_IDLE;
               	}

               	o_state.field.bits.zero_flag = 1;
               }

               o_state.field.bits.ones_cnt = 0;
               o_state.field.bits.nibble = 0x0;
	            o_msk = 0x01;
            }
            else // ones_cnt < 6 : not a new flag detection!
            {
               if ( o_state.field.bits.open_flag )
               {
                  if ( o_state.field.bits.ones_cnt != 5 ) //no zero deletion?
                  {
                     o_state.field.bits.zero_flag = 1;

                     //
                     // insert the bit '0' into extracted bits:

                     o_state.field.bits.nibble &= ~o_msk;
                     o_msk <<= 1;
                  }
                  else // zero bit deletion!
                  {
                     zero_del = TRUE;

                     //
                     // delete(ignore) the bit '0':

                     o_state.field.bits.zero_flag = 0;
                  }
               }
               else //no openning flag yet!
               {
                  o_state.field.bits.zero_flag = 1;
               }
            }

            o_state.field.bits.ones_cnt = 0;
         } //if (i_bit)
      } //for nibble bits

      //
      // calculate the num of bits to be inserted:

      num_bits = 0;

      while ( o_msk >>= 1 ) num_bits++;

      if ( num_bits <= 4 )
      {
         if ( o_state.field.bits.command == HDLCR_CMD_IDLE && num_bits > 0 )
         {
         	if ( num_bits == 3 )  //zero-bit deleted insertion
         		o_state.field.bits.command = HDLCR_CMD_INS3;
         	else if ( num_bits == 4 )
         		o_state.field.bits.command = HDLCR_CMD_INS4;
         	else
         		o_state.field.bits.command = HDLCR_CMD_INVALID;

         }
         else if ( o_state.field.bits.command == HDLCR_CMD_BOP0 )
         {
            o_state.field.bits.command = num_bits <= 3 ? HDLCR_CMD_BOP0 + num_bits :
                                                         HDLCR_CMD_INVALID;
         }
      }
      else //invalid number of bits to insert!
      {
         o_state.field.bits.command = HDLCR_CMD_INVALID;
      }

      if ( comments_on )
      {
         fprintf ( fp, "/*  %04x : %c%c%c%c  %d %d %d  ....  %04x : %s  %c%c%c%c  %d %d %d   %c  */",
                        i_state.value,
                        '0' + ( ( i_state.field.bits.nibble >> 3 ) & 0x1 ),
                        '0' + ( ( i_state.field.bits.nibble >> 2 ) & 0x1 ),
                        '0' + ( ( i_state.field.bits.nibble >> 1 ) & 0x1 ),
                        '0' + ( ( i_state.field.bits.nibble >> 0 ) & 0x1 ),
                        i_state.field.bits.zero_flag,
                        i_state.field.bits.open_flag,
                        i_state.field.bits.ones_cnt,
                        o_state.value,
                        hdlc_cmd_name [ o_state.field.bits.command ],
                        num_bits >= 4 ? ( '0' + ( ( o_state.field.bits.nibble >> 3 ) & 0x1 ) ) : '-',
                        num_bits >= 3 ? ( '0' + ( ( o_state.field.bits.nibble >> 2 ) & 0x1 ) ) : '-',
                        num_bits >= 2 ? ( '0' + ( ( o_state.field.bits.nibble >> 1 ) & 0x1 ) ) : '-',
                        num_bits >= 1 ? ( '0' + ( ( o_state.field.bits.nibble >> 0 ) & 0x1 ) ) : '-',
                        o_state.field.bits.zero_flag,
                        o_state.field.bits.open_flag,
                        o_state.field.bits.ones_cnt,
                        zero_del ? 'y' : 'n'
                        );
      }

      if ( i_state.value < HDLCR_ISTATE_MAXVAL - 1 )
         fprintf ( fp, "  0x%04x,\n", o_state.value );
      else
         fprintf ( fp, "  0x%04x\n", o_state.value );

   } //for states

   fclose ( fp );
   printf ("ok.\n");
   return 0;
}
//---------------------------------------------------------------------------


