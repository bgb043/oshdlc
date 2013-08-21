#ifndef __HDLC_LIB_H
#define __HDLC_LIB_H

#include "types.h"
#include "hdlctrans.h"
#include "hdlcrec.h"
#include "dque.h"

/* Maximum allowed packet length for the input
 * to the transmitter!
 */
#define HDLC_PKT_MAXLEN				256

/* HDLC Buffer Capacity Orders (N)
 * ===============================
 * NOTICE that these buffer capacities (2^N) must be
 * larger than HDLC_PKT_MAXLEN.
 */
#define HDLCT_OBUF_CAPORDER		9
#define HDLCR_OBUF_CAPORDER		9


/* HDLC Encoder maximum output packet size
 * as a function of the input packet size.
 */
#define HDLCENCSIZE( n ) 		( (UINT16)( ( (n) + 4 ) * 1.2 ) )

/* HDLC Packet Linked List Structure:
 */
typedef struct
{
	DQUE_Elem qelem;

	int size;
	UINT8 data [ 1 ];

} hdlc_packet_t;


/* HDLC Channel Structure:
 */
typedef struct
{
	hdlctrans_t hdlct;
	DEFINE_HDLCBUF ( hdlct_obuf, HDLCT_OBUF_CAPORDER );  //hdlc transmitter output queue

	hdlcrec_t hdlcr;
	DEFINE_HDLCBUF ( hdlcr_obuf, HDLCR_OBUF_CAPORDER );  //HDLC receiver output queue

	hdlc_packet_t rec_buf; //reception packet queue

	UINT8 r_packet [ HDLC_PKT_MAXLEN ]; //currently received packet
	int r_pi;                           //and its index.

} hdlc_chan_t;


/* Member Functions :
 */

extern hdlc_chan_t *HDLC_createChannel ( void );
extern void HDLC_destroyChannel ( hdlc_chan_t **p_hdlc );
extern void HDLC_resetChannel ( hdlc_chan_t *p_hdlc );
extern int HDLC_encode ( hdlc_chan_t *p_hdlc, const UINT8 inp[ ], int i_size, UINT8 out[ ], int o_cap );
extern int HDLC_decode ( hdlc_chan_t *p_hdlc, const UINT8 inp[ ], int i_size );
extern hdlc_packet_t *HDLC_getDecodedPacket ( hdlc_chan_t *p_hdlc );
extern void HDLC_freePacket ( hdlc_packet_t ** );


#endif /*__HDLC_LIB_H*/

