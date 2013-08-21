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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <time.h>

#include "hdlctmaker.h"
#include "version.h"


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

int hdlct_maker ( char out_file_name[], BOOL comments_on )
{
	FILE *fp;
	hdlct_input_state_t i_state;

   printf ("HDLC Transmitter Maker starts...\n");

   if ( ( fp = fopen ( out_file_name, "w" ) ) == NULL )
   {
      printf ("ERROR: unabled to open file \"%s\"\n", out_file_name);
      return  -1;
   }

   fprintf ( fp, "/********************************************************************\\\n" );
   fprintf ( fp, "  HDLC Transmitter Maker - Version: %u.%u.%u\n",
                              VERSION_MAJOR, VERSION_MINOR, VERSION_BUILT );

   fprintf ( fp, "  Built @%s-%s\n\n", __DATE__, __TIME__ );

   if ( comments_on )
   {
      fprintf ( fp, "  Abbreviations:\n" );
      fprintf ( fp, "  --------------\n" );
      fprintf ( fp, "     STx   : input/output state of the HDLC receiver (hex notation)\n" );
      fprintf ( fp, "     T     : tranparent mode flag (%d bits)\n", HDLCT_NBITS_TRANSFLAG );
      fprintf ( fp, "     C     : ones' count (%d bits)\n", HDLCT_NBITS_ONESCNT );
      fprintf ( fp, "     NIBB  : input nibble (%d bits)\n", HDLCT_NBITS_NIBBLE );
      fprintf ( fp, "     GBITS : generated bits after zero insertion (%d bits)\n", HDLCT_NBITS_GBITS );
      fprintf ( fp, "     I     : zero bit insertion flag (%d bits)\n", HDLCT_NBITS_ZBIF );
      fprintf ( fp, "\n" );
      fprintf ( fp, "  Note-1: All the input states with suffix '!' are so called impossible states!\n" );
      fprintf ( fp, "          Note that ones' count greater than or equal to 5 is impossible!\n" );
      fprintf ( fp, "  Note-2: Unless otherwise specified, all '-' are numerically equal to 0.\n" );
   }

   fprintf ( fp, "\\********************************************************************/\n\n" );

   if ( comments_on )
   {
      fprintf ( fp, "/*  STi: [ NIBB  T C ] -->  STo: [ GBITS  I C ] */\n" );
      fprintf ( fp, "/*  ====   ====  = =        ====   =====  = =   */\n" );
   }

   for ( i_state.value = 0; i_state.value < HDLCT_ISTATE_MAXVAL; i_state.value++ )
   {
      hdlct_output_state_t o_state;
      UINT8 i_nibble = i_state.field.nibble,
                      o_msk = 0x01, i_msk;

      o_state.value = 0;

      if ( i_state.field.ones_cnt < 5 ) //possible state
      {
         o_state.field.ones_cnt = i_state.field.ones_cnt;

         for ( i_msk = 0x01; i_msk != 0x10; i_msk <<= 1 )
         {
            UINT8 i_bit = i_nibble & i_msk;

            if ( i_state.field.trans_flag ) //tranparent mode?
            {
               if ( i_bit )
                  o_state.field.bits |= o_msk;

               o_msk <<= 1;
               o_state.field.ones_cnt = 0;
               o_state.field.zbi_flag = 0;
            }
            else //non-transparent
            {
               if ( i_bit ) //bit '1'
               {
                  o_state.field.bits |= o_msk;
                  o_state.field.ones_cnt++;
                  o_msk <<= 1;

                  if ( o_state.field.ones_cnt == 5 )
                  {
                     o_msk <<= 1; //zero bit insertion
                     o_state.field.zbi_flag = 1;
                     o_state.field.ones_cnt = 0;
                  }
               }
               else //bit '0'
               {
                  o_state.field.ones_cnt = 0;
                  o_msk <<= 1;  //insert '0'
               }
            }
         }

         if ( comments_on )
         {
            fprintf ( fp, "/*  %04x   %c%c%c%c  %d %d  ....  %04x   %c%c%c%c%c  %c %c   */",
                           i_state.value,
                           '0' + ( ( i_state.field.nibble >> 3 ) & 0x1 ),
                           '0' + ( ( i_state.field.nibble >> 2 ) & 0x1 ),
                           '0' + ( ( i_state.field.nibble >> 1 ) & 0x1 ),
                           '0' + ( ( i_state.field.nibble >> 0 ) & 0x1 ),
                           i_state.field.trans_flag,
                           i_state.field.ones_cnt,

                           o_state.value,
                           o_state.field.zbi_flag ? ( '0' + ( ( o_state.field.bits >> 4 ) & 0x1 ) ) : '-',
                           ( '0' + ( ( o_state.field.bits >> 3 ) & 0x1 ) ),
                           ( '0' + ( ( o_state.field.bits >> 2 ) & 0x1 ) ),
                           ( '0' + ( ( o_state.field.bits >> 1 ) & 0x1 ) ),
                           ( '0' + ( ( o_state.field.bits >> 0 ) & 0x1 ) ),
                           i_state.field.trans_flag == 0 ? ( '0' + o_state.field.zbi_flag ) : '-',
                           i_state.field.trans_flag == 0 ? ( '0' + o_state.field.ones_cnt ) : '-'
                           );
         }
      }
      else //impossible state
      {
         if ( comments_on )
         {
            fprintf ( fp, "/*  %04x!  %c%c%c%c  %d %d  ....  ----   -----  - -   */",
                           i_state.value,
                           '0' + ( ( i_state.field.nibble >> 3 ) & 0x1 ),
                           '0' + ( ( i_state.field.nibble >> 2 ) & 0x1 ),
                           '0' + ( ( i_state.field.nibble >> 1 ) & 0x1 ),
                           '0' + ( ( i_state.field.nibble >> 0 ) & 0x1 ),
                           i_state.field.trans_flag,
                           i_state.field.ones_cnt
                           );
         }
      }

      fprintf ( fp, "  0x%04x%c\n", o_state.value,
                  i_state.value < HDLCT_ISTATE_MAXVAL - 1 ? ',' : ' ' );
   }

   fclose ( fp );
   printf ("ok.\n");
   return 0;
}
//---------------------------------------------------------------------------

