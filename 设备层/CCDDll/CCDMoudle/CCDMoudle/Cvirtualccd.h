#pragma once
#include "CCDInterface.h"
class Cvirtualccd:public CCDBase
{
public:
	Cvirtualccd();
	~Cvirtualccd();
public:
	bool   Initialize(const CCDConfig& config);
	bool   IsConnected();
	bool   Connect();
	bool   DisConnect();
	bool   SetExposureTime(double timeMs);
	bool   GetExposureTime(double* timeMs);
	bool   GetPixelNum(int* size);
};

