//SerialPassthrough.h

#include <stdint-gcc.h>

#ifndef PAYLOAD_MAX_SIZE
#define PAYLOAD_MAX_SIZE	128
#endif

#ifndef PASSTHORUGH_RX_BUFFER_SIZE
#define PASSTHROUGH_RX_BUFFER_SIZE		8
#endif

#ifndef PASSTHROUGH_TX_BUFFER_SIZE
#define PASSTHROUGH_TX_BUFFER_SIZE		8
#endif

typedef enum
{
	STATE_STX = 0,
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