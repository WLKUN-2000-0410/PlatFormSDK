#include "Cvirtualccd.h"
#include "CDConfigManager.h"
#include <algorithm>

Cvirtualccd::Cvirtualccd()
	: m_exposureTime(100.0), m_temperature(20.5), m_gain(1),
	m_isConnected(false), m_coolingEnabled(false), m_pixelNum(1024) {
}

Cvirtualccd::~Cvirtualccd() {
}

bool Cvirtualccd::Initialize(const CCDConfig& config) 
{
	for (const auto& file : config.Dependentfiles) 
	{
		if (!file.empty()) 
		{
			LogPrintErr("VIRTUAL device does not support DLL dependencies: {0}",file);
			return false;
		}
	}
	m_exposureTime = config.defaultExposureTime;
	m_temperature = config.defaulttemperature;
	m_gain = config.defaultgain;
	m_isConnected = false;
	m_coolingEnabled = false;
	m_pixelNum = 1024;
	LogPrintInfo("VIRTUAL device initialized with exposure time: {0} , temperature: {1} , gain: {2}",
		m_exposureTime, m_temperature, m_gain);

	return true;
}

bool Cvirtualccd::IsConnected() {
	return m_isConnected;
}

bool Cvirtualccd::Connect() {
	m_isConnected = true;
	LogPrintInfo("VIRTUAL device connected");
	return true;
}

bool Cvirtualccd::DisConnect() {
	m_isConnected = false;
	LogPrintInfo("VIRTUAL device disconnected");
	return true;
}

bool Cvirtualccd::SetExposureTime(double timeMs) {
	if (timeMs <= 0) {
		LogPrintErr("Invalid exposure time: {0}",timeMs);
		return false;
	}
	m_exposureTime = timeMs;
	LogPrintInfo("VIRTUAL device set exposure time to: {0} ms", timeMs);
	return true;
}

bool Cvirtualccd::GetExposureTime(double* timeMs) {
	if (!timeMs) {
		LogPrintErr("Null pointer for exposure time");
		return false;
	}
	*timeMs = m_exposureTime;
	return true;
}

bool Cvirtualccd::SetGain(int gain) {
	if (gain <= 0) {
		LogPrintErr("Invalid gain: {0}",gain);
		return false;
	}
	m_gain = gain;
	LogPrintInfo("VIRTUAL device set gain to: {0}" ,gain);
	return true;
}

bool Cvirtualccd::GetGain(int* gain) {
	if (!gain) {
		LogPrintErr("Null pointer for gain");
		return false;
	}
	*gain = m_gain;
	return true;
}

bool Cvirtualccd::EnableCooling(bool enable) {
	m_coolingEnabled = enable;
	LogPrintInfo("VIRTUAL device cooling {0}" ,(enable ? "enabled" : "disabled"));
	return true;
}

bool Cvirtualccd::SetCoolingTemperature(double temp) {
	m_temperature = temp;
	LogPrintInfo("VIRTUAL device set temperature to: " + std::to_string(temp) + " C");
	return true;
}

bool Cvirtualccd::GetCurrentTemperature(double* temp) {
	if (!temp) {
		LogPrintErr("Null pointer for temperature");
		return false;
	}
	*temp = m_temperature;
	return true;
}
/*
bool Cvirtualccd::DataAcqOneShot(double* pd, int nPixSize) {
	if (!pd || nPixSize <= 0 || nPixSize > m_pixelNum) {
		LogPrintErr("Invalid parameters for DataAcqOneShot: pd=" + std::to_string((uintptr_t)pd) +
			", nPixSize=" + std::to_string(nPixSize));
		return false;
	}
	for (int i = 0; i < nPixSize; ++i) {
		pd[i] = static_cast<double>(i % 256); // Simulated data
	}
	LogPrintInfo("VIRTUAL device acquired one-shot data, size: " + std::to_string(nPixSize));
	return true;
}

bool Cvirtualccd::DataAcqOneShotImg(double* pdImg, int* nPixSize) {
	if (!pdImg || !nPixSize || *nPixSize <= 0 || *nPixSize > m_pixelNum) {
		LogPrintErr("Invalid parameters for DataAcqOneShotImg: pdImg=" + std::to_string((uintptr_t)pdImg) +
			", nPixSize=" + std::to_string(*nPixSize));
		return false;
	}
	for (int i = 0; i < *nPixSize; ++i) {
		pdImg[i] = static_cast<double>(i % 256); // Simulated image data
	}
	LogPrintInfo("VIRTUAL device acquired one-shot image, size: " + std::to_string(*nPixSize));
	return true;
}
*/
bool Cvirtualccd::GetPixelNum(int* size) {
	if (!size) {
		LogPrintErr("Null pointer for pixel number");
		return false;
	}
	*size = m_pixelNum;
	return true;
}