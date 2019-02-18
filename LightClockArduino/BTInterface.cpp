#include "BTInterface.h"
#include "Scheduler.h"

/* CRC-32C (iSCSI) polynomial in reversed bit order. */
//#define POLY 0x82f63b78

/* CRC-32 (Ethernet, ZIP, etc.) polynomial in reversed bit order. */
 #define POLY 0xedb88320 

CBTInterface CBTInterface::Inst;

CBTInterface::CBTInterface() : 
	BTSerial(PIN_A3, 2) // RX | TX
{
	rcvdCmd[0] = 0;
	cmdHeadIndex=0;
	cmdTailIndex=0;
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

uint32_t CBTInterface::CmdBufCRC32(size_t len)
{
	uint32_t crc = 0;
	int k;
	int idx = cmdHeadIndex;

	crc = ~crc;
	while (len--) {
		crc ^= cmdBuffer[idx];
		idx = (idx + 1) % CMD_MAX_SIZE;
		for (k = 0; k < 8; k++)
			crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
	}
	return ~crc;
}


uint32_t CBTInterface::CRC32(const uint8_t *buf, size_t len, uint32_t crc)
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
	//if(*rcvdCmd)
	//{
	//	// If command is complete, just skip all other symbols until we confirm receive and clean up the buffer;
	//	return;
	//}
	if (GetCmdBufLength() == CMD_MAX_SIZE-1)
	{
		Serial.println("!!! Overflow");
		if (!*rcvdCmd)
		{
			// clean up buffer only if there's no pending command
			cmdHeadIndex = 0;
			cmdTailIndex = 0;
		}
		else
		{
			return;
		}
	}

	// Filling in buffer;
	cmdBuffer[cmdTailIndex] = c;
	cmdTailIndex = (cmdTailIndex + 1) % CMD_MAX_SIZE;
}

void CBTInterface::ProcessBTCommands()
{
	Listen();
	if (CheckForCompleteCommand())
	{
		BTPacketHeader* pHeader = (BTPacketHeader*)rcvdCmd;
		switch (pHeader->PacketType)
		{
		case PACK_ScheduleUpdate:
			OnScheduleUpdate(pHeader, rcvdCmd + sizeof(BTPacketHeader));
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
	if (GetCmdBufLength() >= sizeof(BTPacketHeader))
	{
		if (ReadCmdBufferULong(0) != 0xBEEF115E)
		{
			// Look for preamble in buffer
			bool isFound = false;
			for (int i=0; i < GetCmdBufLength() - sizeof(uint32_t);i++)
			{
				if (ReadCmdBufferULong(i) == 0xBEEF115E)
				{
					cmdHeadIndex = (cmdHeadIndex+i)%CMD_MAX_SIZE;
					isFound = true;
					break;
				}
			}
			if (!isFound)
			{
				// Preamble was not found
				return false;
			}
		}

		// Lenght might have been changed after preamble search
		if (GetCmdBufLength() < sizeof(BTPacketHeader))
		{
			return false;
		}

		for (int j = cmdHeadIndex; j != cmdTailIndex; j = (j + 1) % CMD_MAX_SIZE)
		{
			Serial.print(cmdBuffer[j], 16);
			Serial.print(" ");
		}
		Serial.println("");

		uint16_t payloadLength = ReadCmdBufferUShort(10);
		uint32_t fullPacketLength = payloadLength + sizeof(BTPacketHeader) + sizeof(uint32_t);
		if (GetCmdBufLength() >= fullPacketLength)
		{
			Serial.println("Complete");
			uint32_t storedCRC32 = ReadCmdBufferULong(payloadLength + sizeof(BTPacketHeader));
			uint32_t calcCRC32 = CmdBufCRC32(payloadLength + sizeof(BTPacketHeader));
			if (calcCRC32 == storedCRC32)
			{
				Serial.println("!!!Got packet, good CRC");
				int endOfPacketIdx = (cmdHeadIndex + fullPacketLength) % CMD_MAX_SIZE;
				if (endOfPacketIdx > cmdHeadIndex)
				{
					memcpy(rcvdCmd, cmdBuffer+cmdHeadIndex, fullPacketLength);
				}
				else
				{
					memcpy(rcvdCmd, cmdBuffer + cmdHeadIndex, CMD_MAX_SIZE - cmdHeadIndex);
					memcpy(rcvdCmd+(CMD_MAX_SIZE - cmdHeadIndex), cmdBuffer, endOfPacketIdx);
				}
				retVal = true;
			}
			else
			{
				Serial.println("!!!Got packet, bad CRC");
				Serial.println(storedCRC32, 16);
				Serial.println(calcCRC32, 16);
				fullPacketLength = 4;
			}

			// In case of good packet - removing it, we know its lenght is OK
			// In case of bad packet - remove only first 4 bytes (preamble), the rest will be removed while checking for next data chunk
			if (fullPacketLength < GetCmdBufLength())
			{
				cmdHeadIndex = (cmdHeadIndex + fullPacketLength) % CMD_MAX_SIZE;
			}
			else
			{
				cmdHeadIndex = 0;
				cmdTailIndex = 0;
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
		CScheduler::Inst.UpdateScheduleItem(*item);

		// Send back reply
		SendSchedule(pHeader->PacketID);
	}
}

bool CBTInterface::SendSchedule(uint32_t packetID)
{
	uint8_t buffer[sizeof(BTPacketHeader) + sizeof(CScheduleItem)*SCHEDULE_ITEMS_NUM + sizeof(uint32_t)];
	*(BTPacketHeader*)buffer = BTPacketHeader(PACK_ScheduleRecv, packetID);
	CScheduleItem* payload = (CScheduleItem*)(buffer + sizeof(BTPacketHeader));
	for (int i = 0; i < SCHEDULE_ITEMS_NUM; i++)
	{
		payload[i] = CScheduler::Inst.Schedule[i];
	}

	((BTPacketHeader*)buffer)->PayloadLength = sizeof(CScheduleItem)*SCHEDULE_ITEMS_NUM;
	*(uint32_t*)(buffer+sizeof(BTPacketHeader)+sizeof(CScheduleItem)*SCHEDULE_ITEMS_NUM) = CRC32((const uint8_t*)buffer, sizeof(BTPacketHeader) + sizeof(CScheduleItem)*SCHEDULE_ITEMS_NUM);

	Serial.println("...Sending...");
	uint8_t written = BTSerial.write(buffer, sizeof(buffer));
	Serial.println(written);
	return true;
}


bool CBTInterface::SendConfig(CBoardConfig * pConfig)
{
	return false;
}

bool CBTInterface::LoadConfig(CBoardConfig * pConfig)
{
	return false;
}

uint32_t CBTInterface::ReadCmdBufferULong(int offset)
{
	int startIndex = (cmdHeadIndex + offset) % CMD_MAX_SIZE;
	if (startIndex <= CMD_MAX_SIZE - 4)
	{
		return *(uint32_t*)(cmdBuffer + startIndex);
	}
	else
	{
		return ((uint32_t)cmdBuffer[startIndex]) | (((uint32_t)cmdBuffer[(startIndex+1)%CMD_MAX_SIZE])<<8) |
			(((uint32_t)cmdBuffer[(startIndex + 2) % CMD_MAX_SIZE]) << 16) | (((uint32_t)cmdBuffer[(startIndex + 3) % CMD_MAX_SIZE]) << 24);
	}
}

uint16_t CBTInterface::ReadCmdBufferUShort(int offset)
{
	int startIndex = (cmdHeadIndex + offset) % CMD_MAX_SIZE;
	if (startIndex <= CMD_MAX_SIZE - 3)
	{
		return *(uint16_t*)(cmdBuffer + startIndex);
	}
	else
	{
		return ((uint16_t)cmdBuffer[startIndex]) | (((uint16_t)cmdBuffer[(startIndex + 1) % CMD_MAX_SIZE]) << 8);
	}
}