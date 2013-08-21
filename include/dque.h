#ifndef _DQUE_H
#define _DQUE_H

#include "types.h"

typedef struct _DQUE_Elem
{
	struct _DQUE_Elem *prev, *next;

} DQUE_Elem;

typedef DQUE_Elem* DQUE_Handle;

extern void DQUE_New ( DQUE_Handle );
extern void DQUE_Push ( DQUE_Handle, DQUE_Elem * );
extern DQUE_Elem* DQUE_Pop ( DQUE_Handle );
extern void DQUE_Del ( DQUE_Elem * );

#define DQUE_IsEmpty( hQue )  (BOOL)( (hQue)->next == (hQue) )

#endif /*_DQUE_H*/
