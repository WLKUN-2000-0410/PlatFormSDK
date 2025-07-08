#pragma once
#include "CCDInterface.h"

class CCDTypeC2 : public CCDBase
{
public:
	CCDTypeC2();
	~CCDTypeC2();
public:
	bool   Initialize(const CCDConfig& config);
	bool   IsConnected();
	bool   Connect();
	bool   DisConnect();
	bool   SetExposureTime(double timeMs);
	bool   GetExposureTime(double* timeMs);
	bool   GetPixelNum(int* size);
};

