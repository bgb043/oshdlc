#ifndef __LIB_CRC_H
#define __LIB_CRC_H

#define CRC_VERSION     "1.20"

#define CRC_CCITT_INITIAL		0xFFFF	/* initial value of the CRC prior to computation */
#define CRC_CCITT_CHECKVAL		0x1D0F	/* when a sequence and its bit complemented CRC
													 * is passed from the CRC alg.this constant value
													 * is obtained. High byte of the CRC is passed first!
													 */
													 
#define CRC32_INITIAL_VAL	0xffffffffL
#define CRC32_CHECK_VAL		0x00000000L
													 

//extern unsigned short update_crc_16 ( unsigned short crc, char c );
extern unsigned long  update_crc_32 ( unsigned long  crc, char c );
extern unsigned short update_crc_ccitt ( unsigned short crc, char c );
//extern unsigned short update_crc_dnp ( unsigned short crc, char c );

#endif /*__LIB_CRC_H*/
