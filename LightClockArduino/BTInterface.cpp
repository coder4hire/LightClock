#include "BTInterface.h"

/* RFC1952  - 0x04C11DB7 (as in Java) */
/* Here it is in reverced bit order */
#define POLY 0x76DC4190

/* CRC-32C (iSCSI) polynomial in reversed bit order. */
//#define POLY 0x82f63b78

/* CRC-32 (Ethernet, ZIP, etc.) polynomial in reversed bit order. */
/* #define POLY 0xedb88320 */

CBTInterface CBTInterface::Inst;

CBTInterface::CBTInterface() : 
	BTSerial(PIN_A3, 2), // RX | TX
	cmdBufLength(0)
{
	rcvdCmd[0] = 0;
}

CBTInterface::~CBTInterface()
{
}

void CBTInterface::Init()
{
	BTSerial.begin(9600);

	//Now listening for bluetooth
	BTSerial.attachInterrupt(HandleBTChar);
	BTSerial.listen();
}

uint32_t CBTInterface::CRC32(const unsigned char *buf, size_t len, uint32_t crc)
{
	int k;

	crc = ~crc;
	while (len--) {
		crc ^= *buf++;
		for (k = 0; k < 8; k++)
			crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
	}
	return ~crc;
}

void CBTInterface::HandleBTChar(uint8_t c)
{
	CBTInterface::Inst.OnBTCharReceived(c);
}

void CBTInterface::OnBTCharReceived(uint8_t c)
{
	if(*rcvdCmd)
	{
		// If command is complete, just skip all other symbols until we confirm receive and clean up the buffer;
		return;
	}
	if (cmdBufLength >= CMD_MAX_SIZE)
	{
		cmdBufLength = 0;
	}

	// TODO: add check for start sequence
	// Filling in buffer;
	cmdBuffer[cmdBufLength] = c;
	cmdBufLength++;
}

void CBTInterface::ProcessBTCommands()
{
	if (CheckForCompleteCommand())
	{
		BTPacketHeader* pHeader = (BTPacketHeader*)rcvdCmd;
		switch (pHeader->PacketType)
		{
		case PACK_ScheduleUpdate:
			Serial.println("!!!Schedule");
			OnScheduleUpdate(pHeader, cmdBuffer + sizeof(BTPacketHeader));
			break;
		}
		rcvdCmd[0] = 0;
	}
}

bool CBTInterface::CheckForCompleteCommand()
{
	if (*rcvdCmd)
	{
		Serial.println("rcvdCmd is already filled");
		// Command is already in rcvdCmd buffer, do not touch other buffers until it is extracted
		return true;
	}

	bool retVal = false;
	if (cmdBufLength >= sizeof(BTPacketHeader))
	{
		BTPacketHeader* pHeader = (BTPacketHeader*)cmdBuffer;
		if (pHeader->Preamble != 0xBEAF115E)
		{
			// Look for preamble in buffer
			int i = 0;
			for (; i < cmdBufLength - sizeof(uint32_t);i++)
			{
				if (*(uint32_t*)(cmdBuffer + i) == 0xBEAF115E)
				{
					cmdBufLength -= i;
					memmove(cmdBuffer, cmdBuffer + i, cmdBufLength);
					break;
				}
			}
			if (i == cmdBufLength - sizeof(uint32_t))
			{
				// Premble was not found
				return false;
			}
		}

		uint32_t fullPacketLength = pHeader->PayloadLength + sizeof(BTPacketHeader) + sizeof(uint32_t);
		if (cmdBufLength >= fullPacketLength)
		{
			for (int j = 0; j < cmdBufLength; j++)
			{
				Serial.print(cmdBuffer[j], 16);
				Serial.print(" ");
			}
			uint32_t storedCRC32 = *(uint32_t*)(cmdBuffer + pHeader->PayloadLength + sizeof(BTPacketHeader));
			if (CRC32(cmdBuffer, pHeader->PayloadLength) == storedCRC32)
			{
				Serial.println("!!!Got packet, good CRC");
				memcpy(rcvdCmd, cmdBuffer, fullPacketLength);
				retVal = true;
			}
			else
			{
				Serial.println("!!!Got packet, bad CRC");
				Serial.println(storedCRC32,16);
				Serial.println(CRC32(cmdBuffer, pHeader->PayloadLength+ sizeof(BTPacketHeader)),16);
			}

			// Removing packet, no matter whether it was good or bad
			cmdBufLength -= fullPacketLength;
			if (cmdBufLength)
			{
				memmove(cmdBuffer, cmdBuffer + fullPacketLength, cmdBufLength);
			}
		}
	}
	return retVal;
}

void CBTInterface::OnScheduleUpdate(BTPacketHeader* pHeader, void* pPayload)
{
	if (pHeader->PayloadLength == sizeof(CScheduleItem))
	{
		CScheduleItem* item = (CScheduleItem*)pPayload;
		CBoardConfig::Inst.UpdateScheduleItem(*item);
	}
}


bool CBTInterface::SendConfig(CBoardConfig * pConfig)
{
	return false;
}

bool CBTInterface::LoadConfig(CBoardConfig * pConfig)
{
	return false;
}
