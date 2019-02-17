#pragma once

#include "BoardConfig.h"
#include "NeoSWSerial.h"

#define CMD_MAX_SIZE 128

enum EPacketType
{
	PACK_Unknown = 0,
	PACK_ScheduleUpdate,
	PACK_ScheduleRecv
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

	uint32_t CRC32(const uint8_t * buf, size_t len, uint32_t crc=0);
	bool IsConnected;
	bool SendConfig(CBoardConfig * pConfig);
	bool LoadConfig(CBoardConfig * pConfig);

	void Listen(){ if(!BTSerial.isListening()) BTSerial.listen(); }
	void ProcessBTCommands();

protected:
	CBTInterface();

	NeoSWSerial BTSerial;
	uint8_t cmdBuffer[CMD_MAX_SIZE];
	uint8_t rcvdCmd[CMD_MAX_SIZE];
	unsigned short cmdBufLength;

	static void HandleBTChar(uint8_t c);
	void OnBTCharReceived(uint8_t c);

	bool CheckForCompleteCommand();
	void OnScheduleUpdate(BTPacketHeader* pHeader, void* pPayload);
	bool SendSchedule(uint32_t packetID);
};