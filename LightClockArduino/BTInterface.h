#pragma once

#include "BoardConfig.h"

class CBTInterface
{
public:
	CBTInterface();
	~CBTInterface();
	void Init();

	uint32_t CRC32(const unsigned char * buf, size_t len, uint32_t crc=0);
	bool IsConnected;
	bool SendConfig(CBoardConfig * pConfig);
	bool LoadConfig(CBoardConfig * pConfig);
};

