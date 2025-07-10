// CCDMoudle.cpp : 定义 DLL 应用程序的导出函数。
//

// CCDMoudle.cpp : 定义 DLL 应用程序的导出函数。
//
#include "CCDMoudle.h"
#include "CCDInterface.h"
#include <memory>
#include <string>
#include "CCDFactory.h"
#include "CDConfigManager.h"
#include "CCDManager.h"
#include <mutex>
#include <fstream>
#include <ctime>
#include <iomanip>  
#include "GlobalShare.h"

#define MODULE_VERSION "1.0.0" // 

API_CCD_Moudle_H bool ZLX_SDKInit()
{
	std::lock_guard<std::mutex> lock(GlobalShare::g_mutex);
	try 
	{
		if (GlobalShare::g_initialized)
		{
			LogPrintErr("DLL already initialized");
			return false;
		}
		LogPrintInfo("Starting DLL initialization");

		auto& configMgr = CCDConfigManager::GetInstance();

		if (!configMgr.EnsureConfigFile()) {
			LogPrintErr("Failed to load or create config file");
			return false;
		}
		std::string typeStr = configMgr.GetGlobalParameter("DefaultCCDType");
		if (typeStr.empty()) {
			typeStr = "VIRTUAL"; // 如果没有配置，使用默认值
		}

		GlobalShare::g_currentType = StringToCCDType(typeStr);
		LogPrintInfo("Selected CCD type: {0}",typeStr);

		GlobalShare::g_currentDevice = CCDFactory::CreateDevice(GlobalShare::g_currentType);
		if (!GlobalShare::g_currentDevice) {
			LogPrintErr("Failed to create CCD device of type: {0}",typeStr);
			return false;
		}

		CCDConfig config = configMgr.GetDeviceConfig(GlobalShare::g_currentType);
		if (!GlobalShare::g_currentDevice->Initialize(config)) 
		{
			LogPrintErr("Failed to initialize CCD device");
			GlobalShare::g_currentDevice.reset();

			LogPrintInfo("Switch to the virtual device.");
			GlobalShare::g_currentType = CCDType::VIRTUAL;
			typeStr = "VIRTUAL";
			GlobalShare::g_currentDevice = CCDFactory::CreateDevice(GlobalShare::g_currentType);
			if (!GlobalShare::g_currentDevice) {
				LogPrintErr("Failed to create VIRTUAL device");
				return false;
			}
			CCDConfig config = configMgr.GetDeviceConfig(GlobalShare::g_currentType);
			if (!GlobalShare::g_currentDevice->Initialize(config)) {
				LogPrintErr("Failed to initialize VIRTUAL device");
				return false;
			}
		}

		GlobalShare::g_initialized = true;
		LogPrintInfo("DLL initialization completed successfully");
		return true;
	}
	catch (const std::exception& e) {
		LogPrintErr("Exception in InitDll: {0}",std::string(e.what()));
		return false;
	}
	catch (...) {
		LogPrintErr("Unknown exception in InitDll");
		return false;
	}
}

API_CCD_Moudle_H bool ZLX_Connect()
{
	std::lock_guard<std::mutex> lock(GlobalShare::g_mutex);

	try {
		if (!GlobalShare::g_initialized) {
			LogPrintErr("DLL not initialized");
			return false;
		}

		if (!GlobalShare::g_currentDevice) {
			CCDConfigManager::GetInstance().SetLastError(SDK_ERROR_DEVICE_NOT_FOUND, "No CCD device available");
			LogPrintErr("No CCD device available");
			return false;
		}

		if (GlobalShare::g_currentDevice->IsConnected()) {
			LogPrintInfo("Device already connected");
			return true;
		}

		LogPrintInfo("Attempting to connect to CCD device");
		bool result = GlobalShare::g_currentDevice->Connect();

		if (result) {
			LogPrintInfo("Successfully connected to CCD device");
		}
		else {
			CCDConfigManager::GetInstance().SetLastError(SDK_ERROR_DEVICE_NOT_CONNECTED, "Failed to connect to CCD device");
			LogPrintErr("Failed to connect to CCD device");
		}

		return result;

	}
	catch (const std::exception& e) {
		LogPrintErr("Exception in Connect: {0}",std::string(e.what()));
		return false;
	}
	catch (...) {
		LogPrintErr("Unknown exception in Connect");
		return false;
	}
}
API_CCD_Moudle_H bool ZLX_IsConnected()
{
	std::lock_guard<std::mutex> lock(GlobalShare::g_mutex);

	if (!GlobalShare::g_initialized || !GlobalShare::g_currentDevice) 
	{
		return false;
	}
	return GlobalShare::g_currentDevice->IsConnected();
}
API_CCD_Moudle_H bool ZLX_DisConnect()
{
	std::lock_guard<std::mutex> lock(GlobalShare::g_mutex);

	try 
	{
		if (!GlobalShare::g_initialized) {
			LogPrintErr("DLL not initialized");
			return false;
		}

		if (!GlobalShare::g_currentDevice) {
			LogPrintErr("No CCD device available");
			return false;
		}

		if (!GlobalShare::g_currentDevice->IsConnected()) {
			LogPrintInfo("Device already disconnected");
			return true;
		}

		LogPrintInfo("Attempting to disconnect from CCD device");
		bool result = GlobalShare::g_currentDevice->DisConnect();

		if (result) {
			LogPrintInfo("Successfully disconnected from CCD device");
		}
		else {
			LogPrintErr("Failed to disconnect from CCD device");
		}

		return result;

	}
	catch (const std::exception& e) {
		LogPrintErr("Exception in DisConnect: {0}",std::string(e.what()));
		return false;
	}
	catch (...) {
		LogPrintErr("Unknown exception in DisConnect");
		return false;
	}
}
API_CCD_Moudle_H bool ZLX_SetExposureTime(double timeMs)
{
	std::lock_guard<std::mutex> lock(GlobalShare::g_mutex);

	try {
		if (!GlobalShare::g_initialized) {
			LogPrintErr("DLL not initialized");
			return false;
		}

		if (!GlobalShare::g_currentDevice) {
			LogPrintErr("No CCD device available");
			return false;
		}

		if (!GlobalShare::g_currentDevice->IsConnected()) {
			LogPrintErr("Device not connected");
			return false;
		}

		if (timeMs <= 0) {
			CCDConfigManager::GetInstance().SetLastError(SDK_ERROR_INVALID_EXPOSURE_TIME,"Invalid exposure time setting");
			LogPrintErr("Invalid exposure time: {0}" ,timeMs);
			return false;
		}

		LogPrintInfo("Setting exposure time to: {0} ms",timeMs);
		bool result = GlobalShare::g_currentDevice->SetExposureTime(timeMs);

		if (result) {
			LogPrintInfo("Successfully set exposure time");
		}
		else {
			LogPrintErr("Failed to set exposure time");
		}

		return result;

	}
	catch (const std::exception& e) {
		LogPrintErr("Exception in SetExposureTime: {0}",std::string(e.what()));
		return false;
	}
	catch (...) {
		LogPrintErr("Unknown exception in SetExposureTime");
		return false;
	}
}
API_CCD_Moudle_H bool ZLX_GetExposureTime(double* timeMs)
{
	std::lock_guard<std::mutex> lock(GlobalShare::g_mutex);

	try {
		if (!timeMs) {
			LogPrintErr("Invalid parameter: timeMs is null");
			return false;
		}

		if (!GlobalShare::g_initialized) {
			LogPrintErr("DLL not initialized");
			return false;
		}

		if (!GlobalShare::g_currentDevice) {
			LogPrintErr("No CCD device available");
			return false;
		}

		if (!GlobalShare::g_currentDevice->IsConnected()) {
			LogPrintErr("Device not connected");
			return false;
		}

		bool result = GlobalShare::g_currentDevice->GetExposureTime(timeMs);

		if (result) {
			LogPrintInfo("Current exposure time: {0} ms" ,std::to_string(*timeMs));
		}
		else {
			LogPrintErr("Failed to get exposure time");
		}

		return result;

	}
	catch (const std::exception& e) {
		LogPrintErr("Exception in GetExposureTime: {0} " ,std::string(e.what()));
		return false;
	}
	catch (...) {
		LogPrintErr("Unknown exception in GetExposureTime");
		return false;
	}
}
API_CCD_Moudle_H bool ZLX_GetPixelNum(int* size)
{
	std::lock_guard<std::mutex> lock(GlobalShare::g_mutex);

	try {
		if (!size) {
			LogPrintErr("Invalid parameter: size is null");
			return false;
		}

		if (!GlobalShare::g_initialized) {
			LogPrintErr("DLL not initialized");
			return false;
		}

		if (!GlobalShare::g_currentDevice) {
			LogPrintErr("No CCD device available");
			return false;
		}

		bool result = GlobalShare::g_currentDevice->GetPixelNum(size);

		if (result) {
			LogPrintInfo("Pixel number: {0}",std::to_string(*size));
		}
		else {
			LogPrintErr("Failed to get pixel number");
		}

		return result;

	}
	catch (const std::exception& e) {
		LogPrintErr("Exception in GetPixelNum: {0}",std::string(e.what()));
		return false;
	}
	catch (...) {
		LogPrintErr("Unknown exception in GetPixelNum");
		return false;
	}
}

API_CCD_Moudle_H bool ZLX_SetGain(int gain)
{
	std::lock_guard<std::mutex> lock(GlobalShare::g_mutex);

	try {
		if (!GlobalShare::g_initialized) {
			LogPrintErr("DLL not initialized");
			return false;
		}

		if (!GlobalShare::g_currentDevice) {
			LogPrintErr("No CCD device available");
			return false;
		}
		if (!GlobalShare::g_currentDevice->IsConnected()) {
			LogPrintErr("Device not connected");
			return false;
		}

		if (gain <= 0) {
			CCDConfigManager::GetInstance().SetLastError(SDK_ERROR_INVALID_GAIN, "Invalid gain setting");
			LogPrintErr("Invalid gain: {0}", gain);
			return false;
		}
		LogPrintInfo("Setting gain to: {0}", gain);
		bool result = GlobalShare::g_currentDevice->SetGain(gain);

		if (result) {
			LogPrintInfo("Successfully set gain");
		}
		else {
			LogPrintErr("Failed to set gain");
		}

		return result;

	}
	catch (const std::exception& e) {
		LogPrintErr("Exception in SetGain: {0}" ,std::string(e.what()));
		return false;
	}
	catch (...) {
		LogPrintErr("Unknown exception in GetPixelNum");
		return false;
	}
}

API_CCD_Moudle_H bool ZLX_GetGain(int * gain)
{
	std::lock_guard<std::mutex> lock(GlobalShare::g_mutex);

	try {
		if (!gain)
		{
			LogPrintErr("Invalid parameter: gain is null");
			return false;
		}

		if (!GlobalShare::g_initialized) {
			LogPrintErr("DLL not initialized");
			return false;
		}

		if (!GlobalShare::g_currentDevice) {
			LogPrintErr("No CCD device available");
			return false;
		}
		if (!GlobalShare::g_currentDevice->IsConnected()) {
			LogPrintErr("Device not connected");
			return false;
		}

		bool result = GlobalShare::g_currentDevice->GetGain(gain);

		if (result) {
			LogPrintInfo("Current gain: {0}",*gain);
		}
		else {
			LogPrintErr("Failed to get gain");
		}

		return result;

	}
	catch (const std::exception& e) {
		LogPrintErr("Exception in GetGain: {0}", std::string(e.what()));
		return false;
	}
	catch (...) {
		LogPrintErr("Unknown exception in GetGain");
		return false;
	}
}

API_CCD_Moudle_H bool ZLX_EnableCooling(bool enable)
{
	std::lock_guard<std::mutex> lock(GlobalShare::g_mutex);

	try {
		if (!GlobalShare::g_initialized) {
			LogPrintErr("DLL not initialized");
			return false;
		}

		if (!GlobalShare::g_currentDevice) {
			LogPrintErr("No CCD device available");
			return false;
		}
		if (!GlobalShare::g_currentDevice->IsConnected()) {
			LogPrintErr("Device not connected");
			return false;
		}

		bool result = GlobalShare::g_currentDevice->EnableCooling(enable);

		if (result) {
			LogPrintInfo("Successfully set EnableCooling");
		}
		else {
			LogPrintErr("Failed to set EnableCooling");
		}

		return result;

	}
	catch (const std::exception& e) {
		LogPrintErr("Exception in EnableCooling: {0}", std::string(e.what()));
		return false;
	}
	catch (...) {
		LogPrintErr("Unknown exception in EnableCooling");
		return false;
	}
}

API_CCD_Moudle_H bool ZLX_SetCoolingTemperature(double temp)
{
	std::lock_guard<std::mutex> lock(GlobalShare::g_mutex);

	try {
		if (!GlobalShare::g_initialized) {
			LogPrintErr("DLL not initialized");
			return false;
		}

		if (!GlobalShare::g_currentDevice) {
			LogPrintErr("No CCD device available");
			return false;
		}
		if (!GlobalShare::g_currentDevice->IsConnected()) {
			LogPrintErr("Device not connected");
			return false;
		}

		if (temp <= 0 || temp >=25) {
			CCDConfigManager::GetInstance().SetLastError(SDK_ERROR_INVALID_TEMPERATURE, "Invalid Temperature setting");
			LogPrintErr("Invalid Temperature: {0}", temp);
			return false;
		}
		LogPrintInfo("Setting Temperature to: {0}", temp);
		bool result = GlobalShare::g_currentDevice->SetCoolingTemperature(temp);

		if (result) {
			LogPrintInfo("Successfully set Temperature");
		}
		else {
			LogPrintErr("Failed to set Temperature");
		}

		return result;

	}
	catch (const std::exception& e) {
		LogPrintErr("Exception in SetTemperature: {0}", std::string(e.what()));
		return false;
	}
	catch (...) {
		LogPrintErr("Unknown exception in SetTemperature");
		return false;
	}
}

API_CCD_Moudle_H bool ZLX_GetCurrentTemperature(double * temp)
{
	std::lock_guard<std::mutex> lock(GlobalShare::g_mutex);

	try {
		if (!temp) {
			LogPrintErr("Invalid parameter: Temperature is null");
			return false;
		}
		if (!GlobalShare::g_initialized) {
			LogPrintErr("DLL not initialized");
			return false;
		}

		if (!GlobalShare::g_currentDevice) {
			LogPrintErr("No CCD device available");
			return false;
		}
		if (!GlobalShare::g_currentDevice->IsConnected()) {
			LogPrintErr("Device not connected");
			return false;
		}

		bool result = GlobalShare::g_currentDevice->GetCurrentTemperature(temp);

		if (result) {
			LogPrintInfo("Current Temperature: {0}",*temp);
		}
		else {
			LogPrintErr("Failed to get Temperature");
		}

		return result;

	}
	catch (const std::exception& e) {
		LogPrintErr("Exception in GetTemperature: {0}", std::string(e.what()));
		return false;
	}
	catch (...) {
		LogPrintErr("Unknown exception in GetTemperature");
		return false;
	}
}

API_CCD_Moudle_H bool ZLX_DataAcqOneShot(unsigned short *pd, unsigned long  nPixSize)
{
	std::lock_guard<std::mutex> lock(GlobalShare::g_mutex);

	try {
		if (!pd) {
			LogPrintErr("Invalid parameter: pd is null");
			return false;
		}

		if (nPixSize <= 0) {
			CCDConfigManager::GetInstance().SetLastError(SDK_ERROR_INVALID_PARAM, "Invalid pixel size");
			LogPrintErr("Invalid pixel size: {0}", nPixSize);
			return false;
		}

		if (!GlobalShare::g_initialized) {
			LogPrintErr("DLL not initialized");
			return false;
		}

		if (!GlobalShare::g_currentDevice) {
			CCDConfigManager::GetInstance().SetLastError(SDK_ERROR_DEVICE_NOT_FOUND, "No CCD device available");
			LogPrintErr("No CCD device available");
			return false;
		}

		if (!GlobalShare::g_currentDevice->IsConnected()) {
			CCDConfigManager::GetInstance().SetLastError(SDK_ERROR_DEVICE_NOT_CONNECTED, "Device not connected");
			LogPrintErr("Device not connected");
			return false;
		}

		int devicePixelNum = 0;
		if (!GlobalShare::g_currentDevice->GetPixelNum(&devicePixelNum)) {
			CCDConfigManager::GetInstance().SetLastError(SDK_ERROR_DEVICE_BUSY, "Failed to get device pixel number");
			LogPrintErr("Failed to get device pixel number");
			return false;
		}

		if (nPixSize < devicePixelNum) {
			CCDConfigManager::GetInstance().SetLastError(SDK_ERROR_BUFFER_TOO_SMALL, "Buffer too small for pixel data");
			LogPrintErr("Buffer too small: provided {0}, required {1}", nPixSize, devicePixelNum);
			return false;
		}

		LogPrintInfo("Starting single-shot data acquisition, pixel count: {0}", devicePixelNum);

		// 执行单次数据采集
		bool result = GlobalShare::g_currentDevice->DataAcqOneShot(pd, nPixSize);

		if (result) {
			LogPrintInfo("Successfully completed single-shot data acquisition");
		}
		else {
			CCDConfigManager::GetInstance().SetLastError(SDK_ERROR_ACQUISITION_FAILED, "Data acquisition failed");
			LogPrintErr("Failed to acquire data");
		}

		return result;

	}
	catch (const std::exception& e) {
		LogPrintErr("Exception in DataAcqOneShot: {0}", std::string(e.what()));
		CCDConfigManager::GetInstance().SetLastError(SDK_ERROR_UNKNOWN, "Exception occurred during data acquisition");
		return false;
	}
	catch (...) {
		LogPrintErr("Unknown exception in DataAcqOneShot");
		CCDConfigManager::GetInstance().SetLastError(SDK_ERROR_UNKNOWN, "Unknown exception occurred");
		return false;
	}
}

API_CCD_Moudle_H bool ZLX_DataAcqOneShotImg(double * pdImg, int * nPixSize)
{
	return false;
}

API_CCD_Moudle_H bool ZLX_GetSDKVersion(char * version)
{
	std::lock_guard<std::mutex> lock(GlobalShare::g_mutex);
	if (!version) {
		LogPrintErr("Null pointer for version");
		return false;
	}
	strncpy_s(version, strlen(MODULE_VERSION) + 1, MODULE_VERSION, strlen(MODULE_VERSION));
	LogPrintInfo("Retrieved SDK version: {0}",std::string(MODULE_VERSION));
	return true;
}

API_CCD_Moudle_H bool ZLX_SDKCleanup()
{
	std::lock_guard<std::mutex> lock(GlobalShare::g_mutex);

	try {
		if (!GlobalShare::g_initialized) {
			return true; // 已经清理过了
		}

		LogPrintInfo("Starting DLL cleanup");

		// 断开设备连接
		if (GlobalShare::g_currentDevice && GlobalShare::g_currentDevice->IsConnected()) {
			GlobalShare::g_currentDevice->DisConnect();
		}

		// 清理设备实例
		GlobalShare::g_currentDevice.reset();

		// 清理管理器
		CCDManager::GetInstance().ClearAllDevices();

		GlobalShare::g_initialized = false;
		GlobalShare::g_currentType = CCDType::VIRTUAL;

		LogPrintInfo("DLL cleanup completed");
		return true;

	}
	catch (const std::exception& e) {
		LogPrintErr("Exception in UnInitDll: {0}",std::string(e.what()));
		return false;
	}
	catch (...) {
		LogPrintErr("Unknown exception in UnInitDll");
		return false;
	}
}

API_CCD_Moudle_H const char* ZLX_GetErrorMessage(SDKErrorCode errorCode) {
	switch (errorCode) {
	case SDK_SUCCESS:
		return "Operation successful";
	case SDK_ERROR_INVALID_PARAM:
		return "Invalid parameter";
	case SDK_ERROR_MEMORY_ALLOC:
		return "Memory allocation failed";
	case SDK_ERROR_NETWORK:
		return "Network error";
	case SDK_ERROR_TIMEOUT:
		return "Operation timed out";
	case SDK_ERROR_DEVICE_NOT_FOUND:
		return "CCD device not found";
	case SDK_ERROR_DEVICE_NOT_CONNECTED:
		return "Device not connected";
	case SDK_ERROR_DEVICE_BUSY:
		return "Device is busy, please try again later";
	case SDK_ERROR_COOLING_FAILED:
		return "Cooling system failure";
	case SDK_ERROR_ACQUISITION_FAILED:
		return "Data acquisition failed";
	case SDK_ERROR_INVALID_TEMPERATURE:
		return "Temperature setting out of range";
	case SDK_ERROR_INVALID_GAIN:
		return "Gain value out of range";
	case SDK_ERROR_INVALID_EXPOSURE_TIME:
		return "Invalid exposure time setting";
	case SDK_ERROR_BUFFER_TOO_SMALL:
		return "Buffer too small";
	case SDK_ERROR_UNKNOWN:
	default:
		return "Unknown error";
	}
}

API_CCD_Moudle_H SDKErrorCode ZLX_GetSdkLastError()
{
	return GlobalShare::g_s_lastError;
}

API_CCD_Moudle_H const char * ZLX_GetLastErrorMessage()
{
	return GlobalShare::g_s_lastErrorMessage;
}

API_CCD_Moudle_H void ZLX_ClearError()
{
	GlobalShare::g_s_lastError = SDK_SUCCESS;
	GlobalShare::g_s_lastErrorMessage[0] = '\0';
}


