#pragma once

#include "BoardConfig.h"
#include "NeoSWSerial.h"

#define CMD_MAX_SIZE 128

struct BTPacketHeader
{
	uint32_t Preamble;
	uint32_t PacketID;
	uint16_t PacketType;
	uint16_t PayloadLength;
};

class CBTInterface
{
public:
	enum EPacketType
	{
		PACK_Unknown = 0,
		PACK_ScheduleUpdate,
		PACK_ScheduleRecv
	};

public:
	static CBTInterface Inst;
	~CBTInterface();
	void Init();

	uint32_t CRC32(const unsigned char * buf, size_t len, uint32_t crc=0);
	bool IsConnected;
	bool SendConfig(CBoardConfig * pConfig);
	bool LoadConfig(CBoardConfig * pConfig);

	void Listen(){ BTSerial.listen(); }
	void ProcessBTCommands();

protected:
	CBTInterface();

	NeoSWSerial BTSerial;
	unsigned char cmdBuffer[CMD_MAX_SIZE];
	unsigned char rcvdCmd[CMD_MAX_SIZE];
	unsigned short cmdBufLength;

	static void HandleBTChar(uint8_t c);
	void OnBTCharReceived(uint8_t c);

	bool CheckForCompleteCommand();
	void OnScheduleUpdate(BTPacketHeader* pHeader, void* pPayload);
};