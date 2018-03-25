//SerialPasthrough.c
#include "SerialPassthrough.h"

#include <string.h>

PASSTHROUGH_PACKET l_BuildPacket;
uint8_t l_PayloadIndex;

PASSTHROUGH_PACKET rxBuffer[PASSTHROUGH_RX_BUFFER_SIZE];
PASSTHROUGH_PACKET txBuffer[PASSTHROUGH_TX_BUFFER_SIZE];
uint8_t rxIn = 0, rxOut = 0, rxCount = 0;
uint8_t txIn = 0, txOut = 0, txCount = 0;

PASSTHROUGH_STATE l_CurrentState = STATE_STX;

uint8_t l_DevID = 0xFF;
uint8_t l_IsForMe = 0;

void (*serial_putc)(uint8_t c);

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

static uint8_t l_CalculateChecksum(PASSTHROUGH_PACKET *pkt)
{
	uint8_t chksum = 0, i = 0;

	if(!pkt)
		return 0xFF;
	
	chksum += pkt->src;
	chksum += pkt->des;
	chksum += pkt->len;
	for(; i < pkt->len; i++)
		chksum += pkt->payload[i];
		
	return chksum;
}

static uint8_t l_STXStateHandler(uint8_t c)
{
	if(c == PASSTHROUGH_STX)
	{
		memset(&l_BuildPacket, 0, sizeof(PASSTHROUGH_PACKET));
		l_IsForMe = 0;
		l_PayloadIndex = 0xFF;
		
		return 1;
	}
		
	return 0;
}

static uint8_t l_DesStateHandler(uint8_t c)
{
	if(c == l_DevID)
		l_IsForMe = 1;
	
	l_BuildPacket.des = c;
	
	return 1;
}

static uint8_t l_SrcStateHandler(uint8_t c)
{
	l_BuildPacket.src = c;
	return 1;
}

static uint8_t l_LenStateHandler(uint8_t c)
{
	l_BuildPacket.len = c;
	return 1;
}

static uint8_t l_PayloadStateHandler(uint8_t c)
{
	l_PayloadIndex++;
	if(l_PayloadIndex >= l_BuildPacket.len)
		return 1;
		
	l_BuildPacket.payload[l_PayloadIndex] = c;
		
	return 0;
}

static uint8_t l_ChecksumStateHandler(uint8_t c)
{
	l_BuildPacket.checksum = c;
	
	if(l_IsForMe && c == l_CalculateChecksum(&l_BuildPacket))
	{
		l_PushQ(&l_BuildPacket, rxBuffer, &rxIn, PASSTHROUGH_RX_BUFFER_SIZE);
		rxCount++;
	}
	
	return 1;
}

static uint8_t l_ETXStateHandler(uint8_t c)
{
	if(c == PASSTHROUGH_ETX)
		return 1;
		
	return 0;
}

void Passthrough_Background(uint8_t c)
{
	//Allows RX and TX machines to work on the same state
	PASSTHROUGH_STATE tempState = l_CurrentState;
	
	//RX state machine
	switch(l_CurrentState)
	{
		case STATE_STX:
		if(l_STXStateHandler(c))
			tempState++;
		break;
		
		case STATE_DES:
		if(l_DesStateHandler(c))
			tempState++;
		break;
		
		case STATE_SRC:
		if(l_SrcStateHandler(c))
			tempState++;
		break;
		
		case STATE_LEN:
		if(l_LenStateHandler(c))
			tempState++;
		break;
		
		case STATE_PAYLOAD:
		if(l_PayloadStateHandler(c))
			tempState++;
		break;
		
		case STATE_CHECKSUM:
		if(l_ChecksumStateHandler(c))
			tempState++;
		break;
		
		case STATE_ETX:
		if(l_ETXStateHandler(c))
			tempState++;
		break;
			
		case STATE_CATCHALL:
		break;
	}
	
	//TX state machine
	switch(l_CurrentState)
	{
		case STATE_STX:
		if(l_CurrentState == tempState) //No increment
		{
			//Add transmit handler
			l_CurrentState = l_CurrentState; //Keep build from failing	
		}
		break;
		
		case STATE_DES:
		if(!l_IsForMe) //Can only start pass through if the packet's not for me
		{
			serial_putc(PASSTHROUGH_STX);
			serial_putc(c);
		}
		break;
		
		case STATE_SRC:
		case STATE_LEN:
		case STATE_PAYLOAD:
		case STATE_CHECKSUM:
		case STATE_ETX:
		if(!l_IsForMe)
			serial_putc(c);
		break;
		
		case STATE_CATCHALL:
		break;
	}
	
	//Copy at the end
	l_CurrentState = tempState;
}