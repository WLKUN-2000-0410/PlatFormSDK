#pragma once
#include "CCDInterface.h"

class CCDTypeC2 : public CCDBase
{
public:
	CCDTypeC2();
	~CCDTypeC2();
public:
	bool   Initialize(const CCDConfig& config) override;
	bool   IsConnected() override;
	bool   Connect() override;
	bool   DisConnect() override;
	bool   SetExposureTime(double timeMs) override;
	bool   GetExposureTime(double* timeMs) override;
	bool   GetPixelNum(int* size) override;
	bool   DataAcqOneShot(unsigned short * buff, unsigned long size) override;

};

