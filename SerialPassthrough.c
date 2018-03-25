//SerialPasthrough.c
#include "SerialPassthrough.h"

#include <string.h>

PASSTHROUGH_PACKET l_BuildPacket;

PASSTHROUGH_PACKET rxBuffer[PASSTHROUGH_RX_BUFFER_SIZE];
PASSTHROUGH_PACKET txBuffer[PASSTHROUGH_TX_BUFFER_SIZE];
uint8_t rxIn = 0, rxOut = 0, rxCount = 0;
uint8_t txIn = 0, txOut = 0, txCount = 0;

static void l_PushQ(PASSTHROUGH_PACKET* pkt, PASSTHROUGH_PACKET* q, uint8_t* qIndex, uint8_t qSize)
{
	memcpy(&q[*qIndex], pkt, sizeof(PASSTHROUGH_PACKET));
	
	(*qIndex)++;
	if(*qIndex >= qSize)
		*qIndex = 0;
}

static void l_PopQ(PASSTHROUGH_PACKET* pkt, PASSTHROUGH_PACKET* q, uint8_t* qIndex, uint8_t qSize)
{
	memcpy(pkt, &q[*qIndex], sizeof(PASSTHROUGH_PACKET));
	
	(*qIndex)++;
	if(*qIndex >= qSize)
		*qIndex = 0;
}

void Passthrough_Background(uint8_t c)
{
	c = c;
}