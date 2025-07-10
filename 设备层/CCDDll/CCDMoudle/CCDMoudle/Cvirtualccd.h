#pragma once
#include "CCDInterface.h"
class Cvirtualccd:public CCDBase
{
public:
	Cvirtualccd();
	~Cvirtualccd();
public:
	bool   Initialize(const CCDConfig& config) override;
	bool   IsConnected() override;
	bool   Connect() override;
	bool   DisConnect() override;
	bool   SetExposureTime(double timeMs) override;
	bool   GetExposureTime(double* timeMs) override;
	bool   GetPixelNum(int* size) override;

	bool   SetGain(int gain) override;
	bool   GetGain(int *gain) override;
	bool   EnableCooling(bool enable) override;
	bool   SetCoolingTemperature(double temp) override;
	bool   GetCurrentTemperature(double* temp) override;
private:
	double		m_exposureTime;
	double		m_temperature;
	int			m_gain;
	bool		m_isConnected;
	bool        m_coolingEnabled;
	int			m_pixelNum;
};

