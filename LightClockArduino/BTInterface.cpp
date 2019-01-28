#include "BTInterface.h"

/* CRC-32C (iSCSI) polynomial in reversed bit order. */
#define POLY 0x82f63b78

/* CRC-32 (Ethernet, ZIP, etc.) polynomial in reversed bit order. */
/* #define POLY 0xedb88320 */


CBTInterface::CBTInterface()
{
}


CBTInterface::~CBTInterface()
{
}

void CBTInterface::Init()
{
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

bool CBTInterface::SendConfig(CBoardConfig * pConfig)
{
	return false;
}

bool CBTInterface::LoadConfig(CBoardConfig * pConfig)
{
	return false;
}
