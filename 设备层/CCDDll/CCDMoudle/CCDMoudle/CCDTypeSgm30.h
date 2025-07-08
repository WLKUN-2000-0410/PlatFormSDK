#pragma once
#include "CCDInterface.h"


class CCDTypeSgm30 : public CCDBase
{
public:
	CCDTypeSgm30();
	~CCDTypeSgm30();
public:
	bool   Initialize(const CCDConfig& config);
	bool   IsConnected();
	bool   Connect();
	bool   DisConnect();
	bool   SetExposureTime(double timeMs);
	bool   GetExposureTime(double* timeMs);
	bool   GetPixelNum(int* size);
};

