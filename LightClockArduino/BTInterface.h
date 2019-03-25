#pragma once

#include "BoardConfig.h"
#include "NeoSWSerial.h"
#include "ScheduleItem.h"

#define CMD_MAX_SIZE 128

enum EPacketType
{
	PACK_Unknown = 0,
	PACK_ScheduleUpdate = 1,
	PACK_GetSchedule = 2,
	PACK_SetTime = 3,
	PACK_GetTime = 4,
	PACK_StopAlarm = 5,
	PACK_EnableScheduleItem = 6,
	PACK_SetVolume = 7,
	PACK_GetConfig = 8,
	PACK_SetConfig = 9,
	PACK_SetManualColor = 0xA,
	PACK_GetSensorsInfo=0xB,
	PACK_PlayMusic = 0xC,
	PACK_StopMusic = 0xD,

	PACK_SimpleAck=0x40,
	PACK_ScheduleRecv=0x41,
	PACK_TimeRecv=0x44,
	PACK_ConfigRecv = 0x48,
	PACK_SensorsInfoRecv = 0x4B
};

struct BTPacketHeader
{
	uint32_t Preamble;
	uint32_t PacketID;
	uint16_t PacketType;
	uint16_t PayloadLength;

	BTPacketHeader() 
	{
		Preamble = 0xBEEF115E;
		PacketID = 0;
		PayloadLength = 0;
	}

	BTPacketHeader(EPacketType packetType, uint32_t packetID=0)
	{
		PacketType = (uint16_t)packetType;
		Preamble = 0xBEEF115E;
		PacketID = packetID;
		PayloadLength = 0;
	}
};

class CBTInterface
{
public:
	static CBTInterface Inst;
	~CBTInterface();
	void Init();

	uint32_t CmdBufCRC32(size_t len);

	uint32_t CRC32(const uint8_t * buf, size_t len, uint32_t crc=0);
	bool IsConnected;

	void Listen(){ if(!BTSerial.isListening()) BTSerial.listen(); }
	void ProcessBTCommands();

protected:
	CBTInterface();

	NeoSWSerial BTSerial;
	uint8_t cmdBuffer[CMD_MAX_SIZE];
	int cmdHeadIndex;
	int cmdTailIndex;
	int GetCmdBufLength() 
	{
		return cmdTailIndex >= cmdHeadIndex ? cmdTailIndex - cmdHeadIndex : CMD_MAX_SIZE - cmdHeadIndex + cmdTailIndex;
	}
	uint32_t ReadCmdBufferULong(int offset);
	uint16_t ReadCmdBufferUShort(int offset);

	uint8_t rcvdCmd[CMD_MAX_SIZE];

	static void HandleBTChar(uint8_t c);
	void OnBTCharReceived(uint8_t c);

	void VisualConfirmation(CScheduleItem::EEffectType type);

	bool CheckForCompleteCommand();

	void OnScheduleUpdate(BTPacketHeader* pHeader, void* pPayload);
	void OnSetTime(BTPacketHeader * pHeader, void * pPayload);
	void OnEnableScheduleItem(BTPacketHeader * pHeader, void * pPayload);
	void OnStopAlarm(BTPacketHeader * pHeader, void * pPayload);
	void OnSetManualColor(BTPacketHeader * pHeader, void * pPayload);
	void OnSetConfig(BTPacketHeader * pHeader, void * pPayload);
	void OnPlayStop(BTPacketHeader * pHeader, bool shouldPlay);
	void OnSetVolume(BTPacketHeader * pHeader, void * pPayload);
	bool SendSensorsInfo(uint32_t packetID);
	bool SendSchedule(uint32_t packetID);
	bool SendTime(uint32_t packetID);
	bool SendSimpleAck(uint32_t packetID);
	bool SendConfig(uint32_t packetID);

	size_t FinalizePacketAndWrite(uint8_t* buffer, EPacketType packetType, uint32_t packetID, uint16_t payloadLength);
};