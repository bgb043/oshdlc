/***************************************************************************
 *   OSHDLC - Open Source HDLC Transceiver                                 *
 *                                                                         *
 *   dque.c - Dynamic Queue manages dynamically allocated structral data   *
 *            by use of a linked list queue (FIFO).                        *
 *                                                                         *
 *   Copyright (C) 2008 DIPCENTER, DIPMAN                                  *
 *   Thanks to TI's DSPBIOS library!..                                     *
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

#include "dque.h"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/* Creates a new queue.
 */
void DQUE_New (
					DQUE_Handle hQue
					)
{
	hQue->next = hQue->prev = hQue;
}
//----------------------------------------------------------------------------

/* Push (insert) a new element (elem) into the queue (hQue).
 */
void DQUE_Push (
						DQUE_Handle  hQue,
						DQUE_Elem*   elem
						)
{
	DQUE_Elem *temp;

	temp = hQue->prev;

	hQue->prev = elem;
	elem->next = hQue;
	elem->prev = temp;
	temp->next = elem;
}
//----------------------------------------------------------------------------

/* Retrieves the first element of the queue.
 * Note that the retrieved element is not deallocated!
 */
DQUE_Elem* DQUE_Pop (
							DQUE_Handle hQue
							)
{
	DQUE_Elem* elem;

	if ( DQUE_IsEmpty ( hQue ) )
		return NULL;

	elem = hQue->next; //head

	hQue->next = elem->next;
	elem->next->prev = hQue;

	return elem;
}
//----------------------------------------------------------------------------

/* Cuts off the pointed element from the queue.
 * Note that the retrieved element is not deallocated!
 */
void DQUE_Del (
						DQUE_Elem* elem
						)
{
	elem->next->prev = elem->prev;
	elem->prev->next = elem->next;
}
//----------------------------------------------------------------------------

