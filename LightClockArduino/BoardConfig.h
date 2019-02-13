#pragma once

#pragma pack(push,1)
class CBoardConfig
{
protected:
	CBoardConfig();
public:
	~CBoardConfig();
	static CBoardConfig Inst;

};
#pragma pack(pop)
