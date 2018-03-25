//SerialPassthrough.h

#include <stdint-gcc.h>

#ifndef PAYLOAD_MAX_SIZE
#define PAYLOAD_MAX_SIZE	128
#endif

#ifndef PASSTHROUGH_RX_BUFFER_SIZE
#define PASSTHROUGH_RX_BUFFER_SIZE		2
#endif

#ifndef PASSTHROUGH_TX_BUFFER_SIZE
#define PASSTHROUGH_TX_BUFFER_SIZE		2
#endif

#define PASSTHROUGH_STX		0xAA
#define PASSTHROUGH_ETX		0x55

typedef enum
{
	STATE_STX = 0,
	STATE_DES,
	STATE_SRC,
	STATE_LEN,
	STATE_PAYLOAD,
	STATE_CHECKSUM,
	STATE_ETX,
	STATE_CATCHALL
}PASSTHROUGH_STATE;

typedef struct  
{
	uint8_t des, src;
	
	uint8_t len;
	uint8_t payload[PAYLOAD_MAX_SIZE];
	
	uint8_t checksum;
}PASSTHROUGH_PACKET;


void Passthrough_Background(uint8_t c);