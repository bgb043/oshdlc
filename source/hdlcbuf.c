/***************************************************************************
 *   OSHDLC - Open Source HDLC Transceiver                                 *
 *                                                                         *
 *   hdlcbuf.c - HDLC buffer mangement routines.                           *
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

#include "hdlcbuf.h"


UINT16 hdlc_buf_get ( hdlc_buf_t *buf )
{
	UINT16 data = buf->data [ buf->ri ];

	buf->ri = ( buf->ri + 1 ) & buf->cap_mask;
	buf->size--;

	return data;
}
//--------------------------------------------------------------------------

BOOL hdlc_buf_full ( hdlc_buf_t *buf )
{
	return (BOOL) ( buf->size == buf->cap_mask );
}
//--------------------------------------------------------------------------

UINT16 hdlc_buf_size ( hdlc_buf_t *buf )
{
	return buf->size;
}
//--------------------------------------------------------------------------

BOOL hdlc_buf_empty ( hdlc_buf_t *buf )
{
	return (BOOL)( buf->size == 0 );
}
//--------------------------------------------------------------------------

void hdlc_buf_put( hdlc_buf_t *buf, const UINT16 val )
{
	buf->data [ buf->wi ] = val;
	buf->wi = ( buf->wi + 1 ) & buf->cap_mask;
	buf->size++;
}
//--------------------------------------------------------------------------
