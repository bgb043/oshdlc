/***************************************************************************
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

#ifndef __TYPES_H
#define __TYPES_H

#include <stdio.h>
#include <stdint.h>
#include <limits.h>


#ifndef __INT8_DEFINED
#define __INT8_DEFINED
	typedef char 		INT8;				/* 8-bit signed integer            */
#endif

#ifndef __INT16_DEFINED
#define __INT16_DEFINED
	typedef short		INT16;			/* 16-bit signed integer            */
#endif

#ifndef __INT32_DEFINED
#define __INT32_DEFINED
	typedef long		INT32;			/* 32-bit signed integer            */
#endif

#ifndef __UINT8_DEFINED
#define __UINT8_DEFINED
	typedef unsigned char		UINT8;        /* 8-bit unsigned integer            */
#endif

#ifndef __UINT16_DEFINED
#define __UINT16_DEFINED
	typedef unsigned short 	UINT16;       /* 16-bit unsigned integer            */
#endif

#ifndef __UINT32_DEFINED
#define __UINT32_DEFINED
	typedef unsigned long		UINT32;       /* 32-bit unsigned integer            */
#endif

#ifndef __BOOL_DEFINED
#define __BOOL_DEFINED
	typedef enum { FALSE = 0, TRUE = !FALSE } BOOL;			  /* boolean type */
#endif

#ifndef __HINIBBLE_DEFINED
#define __HINIBBLE_DEFINED
	#define HINIBBLE(b)  ( ( (b) >> 4 ) & 0x0f )
#endif

#ifndef __LONIBBLE_DEFINED
#define __LONIBBLE_DEFINED
	#define LONIBBLE(b)  ( (b) & 0x0f )
#endif

#ifndef __HIBYTE_DEFINED
#define __HIBYTE_DEFINED
	#define HIBYTE(w)		( ( (w) >> 8 ) & 0xff )
#endif

#ifndef __LOBYTE_DEFINED
#define __LOBYTE_DEFINED
	#define LOBYTE(w)		( (w) & 0xff )
#endif

#ifndef __DIM_DEFINED
#define __DIM_DEFINED
	#define DIM(x)  ( sizeof ( x ) / sizeof ( (x)[0] ) )
#endif


#endif  /*__TYPES_H*/
