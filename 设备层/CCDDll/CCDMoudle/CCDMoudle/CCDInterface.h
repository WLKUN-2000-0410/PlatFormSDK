#pragma once
#include "CCDTypes.h"

class CCDBase
{
public:
	CCDBase() = default;
	virtual ~CCDBase() = default;

public:
	virtual bool   Initialize(const CCDConfig& config) = 0;
	virtual bool   IsConnected() = 0;
	virtual bool   Connect() = 0;
	virtual bool   DisConnect() = 0;
	virtual bool   SetExposureTime(double timeMs) = 0;
	virtual bool   GetExposureTime(double* timeMs) = 0;
	virtual bool   GetPixelNum(int* size) = 0;

	virtual bool   SetCoolingTemperature(double temp) { return false; };
	virtual bool   GetCurrentTemperature(double* temp) { return false;};
	virtual bool   EnableCooling(bool enable) { return false;};

	virtual bool   SetGain(int gain) { return false; };
	virtual bool   GetGain(int *gain) { return false; };

	virtual bool   SetBinning(int hBin, int vBin) { return false; };
	virtual bool   GetBinning(int* hBin, int* vBin) { return false; };


	virtual bool   EnableFVB(bool enable) { return false; };
	virtual bool   IsFVBEnabled() { return false; };
	virtual bool   SetFVBReadoutSpeed(int speed) { return false; };
	virtual int    GetFVBReadoutSpeed() { return false; };

	
};
