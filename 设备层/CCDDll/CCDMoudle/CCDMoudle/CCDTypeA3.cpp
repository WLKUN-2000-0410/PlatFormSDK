#include "CCDTypeA3.h"
#include "CDConfigManager.h" // For logging
#include <fstream>

CCDTypeA3::CCDTypeA3() :
	m_hModule(nullptr),
	m_isConnected(false),
	m_deviceId(-1),
	m_pixelNum(0),
	m_dfield_open(nullptr),
	m_dfield_close(nullptr),
	m_dfield_is_open(nullptr)
	
{
	m_dfield_open = nullptr;
	m_dfield_close = nullptr;
	m_dfield_is_open = nullptr;
	m_dfield_get_detector_size = nullptr;
	m_dfield_set_exposure = nullptr;
	m_dfield_get_exposure = nullptr;
	m_dfield_capture = nullptr;
	m_dfield_get_data = nullptr;
	m_dfield_set_gain = nullptr;
	m_dfield_get_gain = nullptr;
	m_dfield_start_cooling = nullptr;
	m_dfield_stop_cooling = nullptr;
	m_dfield_set_temperature = nullptr;
	m_dfield_get_real_temperature = nullptr;
	m_dfield_usb_mode = nullptr;
	m_dfield_get_bin = nullptr;
	m_dfield_set_readout_mode = nullptr;
}

CCDTypeA3::~CCDTypeA3() 
{
	// 释放所有加载的DLL
	LogPrintInfo("卸载模块相关库：A3 -> {0}", bool(FreeLibrary(m_hModule)));
}

bool CCDTypeA3::Initialize(const CCDConfig& config)
{
	for (std::vector<std::string>::const_iterator it = config.Dependentfiles.begin();
		it != config.Dependentfiles.end(); ++it)
	{
		if (it->empty()) {
			continue;
		}
		std::ifstream file(*it);
		if (!file.good()) {
			LogPrintErr("Dependency file not found: {0}", *it);
			return false;
		}
		file.close();
	}
	if (initDll())
	{
	/*	m_exposureTime = config.defaultExposureTime;
		m_temperature = config.defaulttemperature;
		m_gain = config.defaultgain;
		m_isConnected = false;*/
		return true;
	}
	return false;
}

bool CCDTypeA3::initDll()
{
	m_hModule = LoadLibraryA("dfield.dll");
	if (!m_hModule) {
		LogPrintErr("Failed to load dfield.dll. Error code: {0}", GetLastError());
		return false;
	}
	loadDllFun();
	return true;
}
void CCDTypeA3::loadDllFun()
{
	// Use GetProcAddress to get the address of each function from the DLL
	m_dfield_open = (dfield_open_t)GetProcAddress(m_hModule, "dfield_open");
	m_dfield_close = (dfield_close_t)GetProcAddress(m_hModule, "dfield_close");
	m_dfield_is_open = (dfield_is_open_t)GetProcAddress(m_hModule, "dfield_is_open");
	m_dfield_get_detector_size = (dfield_get_detector_size_t)GetProcAddress(m_hModule, "dfield_get_detector_size");
	m_dfield_set_exposure = (dfield_set_exposure_t)GetProcAddress(m_hModule, "dfield_set_exposure");
	m_dfield_get_exposure = (dfield_get_exposure_t)GetProcAddress(m_hModule, "dfield_get_exposure");
	m_dfield_capture = (dfield_capture_t)GetProcAddress(m_hModule, "dfield_capture");
	m_dfield_get_data = (dfield_get_data_t)GetProcAddress(m_hModule, "dfield_get_data");
	m_dfield_set_gain = (dfield_set_gain_t)GetProcAddress(m_hModule, "dfield_set_gain");
	m_dfield_get_gain = (dfield_get_gain_t)GetProcAddress(m_hModule, "dfield_get_gain");
	m_dfield_start_cooling = (dfield_start_cooling_t)GetProcAddress(m_hModule, "dfield_start_cooling");
	m_dfield_stop_cooling = (dfield_stop_cooling_t)GetProcAddress(m_hModule, "dfield_stop_cooling");
	m_dfield_set_temperature = (dfield_set_temperature_t)GetProcAddress(m_hModule, "dfield_set_temperature");
	m_dfield_get_real_temperature = (dfield_get_real_temperature_t)GetProcAddress(m_hModule, "dfield_get_real_temperature");
	m_dfield_usb_mode = (dfield_usb_mode_t)GetProcAddress(m_hModule, "dfield_usb_mode");
	m_dfield_set_readout_mode = (dfield_set_readout_mode)GetProcAddress(m_hModule, "dfield_set_readout_mode");
}

bool CCDTypeA3::IsConnected()
{
	if (!m_dfield_is_open || m_deviceId < 0) return false;
	return m_dfield_is_open(m_deviceId);
}

bool CCDTypeA3::Connect()
{
	if (!m_dfield_open) return false;
	//这个千万不能缺少, 会出问题
	m_dfield_usb_mode(false);

	//这个地方com口先写死, 后续可以改成轮询,或从配置文件中读
	m_deviceId = m_dfield_open("COM10");

	if (m_deviceId <= 0) {
		LogPrintErr("Failed to open A3 device.");
		return false;
	}

	m_isConnected = true;

	LogPrintInfo("A3 device connected successfully with ID: {0}", m_deviceId);
	return true;
}

bool CCDTypeA3::DisConnect()
{
	if (!m_isConnected || !m_dfield_close) return false;
	m_dfield_close(m_deviceId);
	m_isConnected = false;
	m_deviceId = -1;
	LogPrintInfo("A3 device disconnected.");
	return true;
}

bool CCDTypeA3::SetExposureTime(double timeMs)
{
	if (!m_isConnected ) return false;
	// Convert from milliseconds (our interface) to microseconds (dfield API)
	int timeUs = static_cast<int>(timeMs * 1000.0);
	return m_dfield_set_exposure(m_deviceId, timeUs);
}

bool CCDTypeA3::GetExposureTime(double* timeMs)
{
	if (!m_isConnected) return false;
	int timeUs = 0;
	bool result = m_dfield_get_exposure(m_deviceId, &timeUs);
	if (result) {
		// Convert from microseconds (dfield API) to milliseconds (our interface)
		*timeMs = static_cast<double>(timeUs) / 1000.0;
	}
	return result;
}
bool CCDTypeA3::GetPixelNum(int* size)
{
	if (!m_isConnected ) return false;
	short width = 0, height = 0;

	m_dfield_get_detector_size(m_deviceId, &width, &height);

	*size = width;//2048

	return width > 0;
}

bool CCDTypeA3::DataAcqOneShot(unsigned short* buff, unsigned long size)
{
	if (!m_isConnected ) return false;

	// 0.Set readout mode
	m_dfield_set_readout_mode(m_deviceId, mode_fvb);

	// 1.Start the capture
	if (!m_dfield_capture(m_deviceId)) {
		LogPrintErr("A3: dfield_capture failed.");
		return false;
	}

	// 2. Get the data
	if (!m_dfield_get_data(m_deviceId, buff, size)) {
		LogPrintErr("A3: dfield_get_data failed.");
		return false;
	}
	return true;
}

bool CCDTypeA3::SetGain(int gain)
{
	if (!m_isConnected ) return false;
	return m_dfield_set_gain(m_deviceId, static_cast<short>(gain));
}

bool CCDTypeA3::GetGain(int* gain)
{
	if (!m_isConnected ) return false;
	short gain_s = 0;
	bool result = m_dfield_get_gain(m_deviceId, &gain_s);
	if (result) {
		*gain = gain_s;
	}
	return result;
}
bool CCDTypeA3::EnableCooling(bool enable)
{
	if (!m_isConnected) return false;
	if (enable) {
		if (!m_dfield_start_cooling) return false;
		return m_dfield_start_cooling(m_deviceId);
	}
	else {
		if (!m_dfield_stop_cooling) return false;
		return m_dfield_stop_cooling(m_deviceId);
	}
}

bool CCDTypeA3::SetCoolingTemperature(double temp)
{
	if (!m_isConnected) return false;
	return m_dfield_set_temperature(m_deviceId, static_cast<float>(temp));
}

bool CCDTypeA3::GetCurrentTemperature(double* temp)
{
	if (!m_isConnected) return false;
	float temp_f = 0.0f;
	bool result = m_dfield_get_real_temperature(m_deviceId, &temp_f);
	if (result) {
		*temp = temp_f;
	}
	return result;
}

