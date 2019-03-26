#include "BTInterface.h"
#include "Scheduler.h"
#include "Main.h"
#include "RGBControl.h"
#include "BoardConfig.h"

/* CRC-32C (iSCSI) polynomial in reversed bit order. */
//#define POLY 0x82f63b78

/* CRC-32 (Ethernet, ZIP, etc.) polynomial in reversed bit order. */
 #define POLY 0xedb88320 

CBTInterface CBTInterface::Inst;

#define BUF_OVERHEAD (sizeof(BTPacketHeader) + sizeof(uint32_t))

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

void CBTInterface::VisualConfirmation(CScheduleItem::EEffectType type)
{
	if (CBoardConfig::Inst.AreVisualConfirmationsEnabled && !CScheduler::Inst.IsCurrentEffectRunning())
	{
		CScheduler::Inst.RunEffectNow(type,1);
	}
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
			VisualConfirmation(CScheduleItem::EF_DARK_GREEN);
			break;
		case PACK_GetSchedule:
			SendSchedule(pHeader->PacketID);
			break;
		case PACK_GetTime:
			SendTime(pHeader->PacketID);
			break;
		case PACK_SetTime:
			OnSetTime(pHeader, rcvdCmd + sizeof(BTPacketHeader));
			VisualConfirmation(CScheduleItem::EF_DARK_GREEN);
			break;
		case PACK_EnableScheduleItem:
			OnEnableScheduleItem(pHeader, rcvdCmd + sizeof(BTPacketHeader));
			VisualConfirmation(CScheduleItem::EF_DARK_GREEN);
			break;
		case PACK_StopAlarm:
			OnStopAlarm(pHeader, rcvdCmd + sizeof(BTPacketHeader));
			VisualConfirmation(CScheduleItem::EF_DARK_BLUE);
			break;
		case PACK_SetVolume:
			OnSetVolume(pHeader, rcvdCmd + sizeof(BTPacketHeader));
			break;
		case PACK_GetConfig:
			CMain::Inst.Player.SetVolume(CBoardConfig::Inst.Volume); // Setting player volume to configured state
			SendConfig(pHeader->PacketID);
			break;
		case PACK_SetConfig:
			OnSetConfig(pHeader, rcvdCmd + sizeof(BTPacketHeader));
			VisualConfirmation(CScheduleItem::EF_DARK_GREEN);
			break;
		case PACK_SetManualColor:
			OnSetManualColor(pHeader, rcvdCmd + sizeof(BTPacketHeader));
			break;
		case PACK_GetSensorsInfo:
			SendSensorsInfo(pHeader->PacketID);
			VisualConfirmation(CScheduleItem::EF_DARK_BLUE);
			break;
		case PACK_PlayMusic:
			OnPlayStop(pHeader,true);
			VisualConfirmation(CScheduleItem::EF_DARK_GREEN);
			break;
		case PACK_StopMusic:
			OnPlayStop(pHeader, false);
			VisualConfirmation(CScheduleItem::EF_DARK_RED);
			break;
		}
	}

	rcvdCmd[0] = 0;
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

		uint16_t payloadLength = ReadCmdBufferUShort(10);
		uint32_t fullPacketLength = payloadLength + sizeof(BTPacketHeader) + sizeof(uint32_t);
		if (GetCmdBufLength() >= fullPacketLength)
		{
			for (int j = cmdHeadIndex; j != cmdTailIndex; j = (j + 1) % CMD_MAX_SIZE)
			{
				Serial.print(cmdBuffer[j], 16);
				Serial.print(" ");
			}
			Serial.println("");

			uint32_t storedCRC32 = ReadCmdBufferULong(payloadLength + sizeof(BTPacketHeader));
			uint32_t calcCRC32 = CmdBufCRC32(payloadLength + sizeof(BTPacketHeader));
			if (calcCRC32 == storedCRC32)
			{
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

void CBTInterface::OnSetTime(BTPacketHeader* pHeader, void* pPayload)
{
	if (pHeader->PayloadLength == sizeof(uint32_t))
	{
		time_t newTime = *(uint32_t*)pPayload;
		CMain::Inst.RTC.adjust(DateTime(newTime));

		// Send back reply
		SendTime(pHeader->PacketID);
	}
}

void CBTInterface::OnEnableScheduleItem(BTPacketHeader* pHeader, void* pPayload)
{
	uint8_t* data = (uint8_t*)pPayload;
	CScheduler::Inst.EnableScheduleItem(data[0], data[1]);
	SendSchedule(pHeader->PacketID);
}

void CBTInterface::OnStopAlarm(BTPacketHeader * pHeader, void * pPayload)
{
	CScheduler::Inst.StopEffects();
	SendSimpleAck(pHeader->PacketID);
}

void CBTInterface::OnSetManualColor(BTPacketHeader * pHeader, void * pPayload)
{
	RGBW color = *(long*)pPayload;
	Serial.print("RGBW:");
	Serial.println(color.Data,16);
	CRGBControl::Inst.SetRGBW(color);
	SendSimpleAck(pHeader->PacketID);
}

void  CBTInterface::OnSetConfig(BTPacketHeader * pHeader, void * pPayload)
{
	CBoardConfigItems* pNewCfg = (CBoardConfigItems*)pPayload;
	if (CBoardConfig::Inst.Volume != pNewCfg->Volume)
	{
		CMain::Inst.Player.SetVolume(pNewCfg->Volume);
	}
	*CBoardConfig::Inst.GetItemsPtr() = *pNewCfg;
	CBoardConfig::Inst.StoreToEEPROM();
	SendSimpleAck(pHeader->PacketID);
}

void CBTInterface::OnSetVolume(BTPacketHeader * pHeader, void * pPayload)
{
	int newVolume = *(unsigned char*)pPayload;
	CMain::Inst.Player.SetVolume(newVolume);
	SendSimpleAck(pHeader->PacketID);
}

void CBTInterface::OnPlayStop(BTPacketHeader * pHeader, bool shouldPlay)
{
	if (shouldPlay)
	{
		CMain::Inst.Player.Play();
	}
	else
	{
		CMain::Inst.Player.Stop();
	}
	SendSimpleAck(pHeader->PacketID);
}


bool CBTInterface::SendSensorsInfo(uint32_t packetID)
{
	uint8_t buffer[BUF_OVERHEAD + sizeof(CSensorsInfo)];
	CSensorsInfo* payload = (CSensorsInfo*)(buffer + sizeof(BTPacketHeader));
	CMain::Inst.GetSensorsInfo(payload);
	size_t written = FinalizePacketAndWrite(buffer, PACK_SensorsInfoRecv, packetID, sizeof(CSensorsInfo));
	return written>0;
}

bool CBTInterface::SendConfig(uint32_t packetID)
{
	uint8_t buffer[BUF_OVERHEAD + sizeof(CBoardConfigItems)];
	CBoardConfigItems* payload = (CBoardConfigItems*)(buffer + sizeof(BTPacketHeader));
	*payload = *CBoardConfig::Inst.GetItemsPtr();
	size_t written = FinalizePacketAndWrite(buffer, PACK_ConfigRecv, packetID, sizeof(CBoardConfigItems));
	return written>0;
}


bool CBTInterface::SendSchedule(uint32_t packetID)
{
	uint8_t buffer[BUF_OVERHEAD + sizeof(CScheduleItem)*SCHEDULE_ITEMS_NUM];
	CScheduleItem* payload = (CScheduleItem*)(buffer + sizeof(BTPacketHeader));
	for (int i = 0; i < SCHEDULE_ITEMS_NUM; i++)
	{
		payload[i] = CScheduler::Inst.Schedule[i];
	}
	size_t written = FinalizePacketAndWrite(buffer, PACK_ScheduleRecv, packetID, sizeof(CScheduleItem)*SCHEDULE_ITEMS_NUM);
	return written>0;
}

bool CBTInterface::SendTime(uint32_t packetID)
{
	uint8_t buffer[BUF_OVERHEAD + sizeof(uint32_t)];
	*(uint32_t*)(buffer + sizeof(BTPacketHeader)) = CMain::Inst.RTC.now().unixtime();
	size_t written = FinalizePacketAndWrite(buffer, PACK_TimeRecv, packetID, sizeof(uint32_t));
	return written > 0;
}

bool CBTInterface::SendSimpleAck(uint32_t packetID)
{
	uint8_t buffer[BUF_OVERHEAD];
	size_t written = FinalizePacketAndWrite(buffer, PACK_SimpleAck, packetID, 0);
	return written > 0;
}

size_t CBTInterface::FinalizePacketAndWrite(uint8_t* buffer,EPacketType packetType, uint32_t packetID, uint16_t payloadLength)
{
	*(BTPacketHeader*)buffer = BTPacketHeader(packetType, packetID);
	((BTPacketHeader*)buffer)->PayloadLength = payloadLength;
	*(uint32_t*)(buffer + sizeof(BTPacketHeader) + payloadLength) = CRC32((const uint8_t*)buffer, sizeof(BTPacketHeader) + payloadLength);
	BTSerial.listen();
	return BTSerial.write(buffer, BUF_OVERHEAD + payloadLength);
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