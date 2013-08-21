/***************************************************************************
 *   OSHDLC - Open Source HDLC Transceiver                                 *
 *                                                                         *
 *   main.c - Builds HDLC transmitter and receiver tables.                 *
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

#include "hdlctmaker.h"
#include "hdlcrmaker.h"

int main ( void )
{
	printf ( "OSHDLC Table Maker\n" );

	hdlct_maker ( "hdlct_table.h", TRUE );
   hdlcr_maker ( "hdlcr_table.h", TRUE );

   printf ("Both tables has been created!\nBYE!\n");
	return 0;
}
