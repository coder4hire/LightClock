#pragma once

#pragma pack(push,1)
struct CBoardConfigItems
{
	unsigned char Volume;
	unsigned short LightThresholdBack;
	bool IsStopLightEnabled;
	bool DoesBackligthDisableRGB;
};

class CBoardConfig : public CBoardConfigItems
{
protected:
	CBoardConfig();
public:
	~CBoardConfig();
	void StoreToEEPROM();
	void LoadFromEEPROM();
	CBoardConfigItems* GetItemsPtr() { return (CBoardConfigItems*)this; }
	static CBoardConfig Inst;
};
#pragma pack(pop)
