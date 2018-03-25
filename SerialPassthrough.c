//SerialPasthrough.c
#include "SerialPassthrough.h"

PASSTHROUGH_PACKET l_BuildPacket;
PASSTHROUGH_PACKET rxBuffer[PASSTHROUGH_RX_BUFFER_SIZE];
PASSTHROUGH_PACKET txBuffer[PASSTHROUGH_TX_BUFFER_SIZE];

void Passthrough_Background(uint8_t c)
{
	c = c;
}