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

#ifndef __HB_VERSION_H
#define __HB_VERSION_H

#define VERSION  VERSION_MAKE ( 1, 1, 0 )

#define VERSION_MAKE(major, minor, built)  ( (unsigned long)( ( ( (unsigned long)(major) ) << 16 ) | ( (minor) << 8 ) | ( (built) ) ) )

#define VERSION_MAJOR   ( (unsigned short)( VERSION >> 16 ) )
#define VERSION_MINOR   ( (unsigned short)( VERSION >> 8 ) & 0xffu )
#define VERSION_BUILT   ( (unsigned short)( VERSION >> 0 ) & 0xffu )

#endif /*__HB_VERSION_H*/
