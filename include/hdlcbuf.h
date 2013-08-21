#ifndef __HDLCBUF_H
#define __HDLCBUF_H

#include "types.h"
#include <string.h>

typedef struct _hdlc_buf_t
{
	UINT16 size;   		//buffer size
	UINT16 cap_mask;		//buffer capacity mask (CARE!!!: this must be 2^N - 1 type)

	UINT16 wi, ri;			//buffer write/read indexes
	UINT16 data [ 1 ];

} hdlc_buf_t;

/*
 * HDLC Queue Macros:
 */

#define DEFINE_HDLCBUF( v_buf, capo )	hdlc_buf_t v_buf; \
											  	   UINT16 v_buf##_ext [ ( 1 << (capo) ) - 1 ]

#define HDLCBUF_GETCAP( v_buf )  ( DIM ( v_buf##_ext ) + 1 )

#define hdlc_buf_init( v_buf ) \
{ \
	memset ( &(v_buf), 0, sizeof ( hdlc_buf_t ) ); \
	(v_buf).cap_mask = DIM ( v_buf##_ext ); \
}

//#define hdlc_buf_size( pb ) 	(pb)->size
/*#define hdlc_buf_empty( pb )  ( (BOOL)( (pb)->wi == (pb)->ri ) )*/
//#define hdlc_buf_empty( pb )  ( (BOOL) ( (pb)->size == 0 ) )
//#define hdlc_buf_full( pb )   ( (BOOL) ( (pb)->size == (pb)->cap_mask ) )
#define hdlc_buf_clear( pb ) \
{ \
	(pb)->wi = (pb)->ri = 0; \
	(pb)->size = 0; \
}

/*#define hdlc_buf_put( buf, val ) \
{ \
	(buf)->data [ (buf)->wi ] = val; \
	(buf)->wi = ( (buf)->wi + 1 ) & (buf)->cap_mask; \
}*/

extern UINT16 hdlc_buf_get ( hdlc_buf_t * );
extern void hdlc_buf_put ( hdlc_buf_t *, const UINT16 );
extern BOOL hdlc_buf_empty ( hdlc_buf_t * );
extern UINT16 hdlc_buf_size ( hdlc_buf_t * );
extern BOOL hdlc_buf_full ( hdlc_buf_t * );

#endif /*__HDLCBUF_H*/
