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

API_CCD_Moudle_H bool SDKInit()
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
		LogPrintInfo("Selected CCD type: " + typeStr);

		GlobalShare::g_currentDevice = CCDFactory::CreateDevice(GlobalShare::g_currentType);
		if (!GlobalShare::g_currentDevice) {
			LogPrintErr("Failed to create CCD device of type: " + typeStr);
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
		LogPrintErr("Exception in InitDll: " + std::string(e.what()));
		return false;
	}
	catch (...) {
		LogPrintErr("Unknown exception in InitDll");
		return false;
	}
}

API_CCD_Moudle_H bool Connect()
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
		LogPrintErr("Exception in Connect: " + std::string(e.what()));
		return false;
	}
	catch (...) {
		LogPrintErr("Unknown exception in Connect");
		return false;
	}
}
API_CCD_Moudle_H bool IsConnected()
{
	std::lock_guard<std::mutex> lock(GlobalShare::g_mutex);

	if (!GlobalShare::g_initialized || !GlobalShare::g_currentDevice) 
	{
		return false;
	}
	return GlobalShare::g_currentDevice->IsConnected();
}
API_CCD_Moudle_H bool DisConnect()
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
		LogPrintErr("Exception in DisConnect: " + std::string(e.what()));
		return false;
	}
	catch (...) {
		LogPrintErr("Unknown exception in DisConnect");
		return false;
	}
}
API_CCD_Moudle_H bool SetExposureTime(double timeMs)
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
			LogPrintErr("Invalid exposure time: " + std::to_string(timeMs));
			return false;
		}

		LogPrintInfo("Setting exposure time to: " + std::to_string(timeMs) + " ms");
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
		LogPrintErr("Exception in SetExposureTime: " + std::string(e.what()));
		return false;
	}
	catch (...) {
		LogPrintErr("Unknown exception in SetExposureTime");
		return false;
	}
}
API_CCD_Moudle_H bool GetExposureTime(double* timeMs)
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
			LogPrintInfo("Current exposure time: " + std::to_string(*timeMs) + " ms");
		}
		else {
			LogPrintErr("Failed to get exposure time");
		}

		return result;

	}
	catch (const std::exception& e) {
		LogPrintErr("Exception in GetExposureTime: " + std::string(e.what()));
		return false;
	}
	catch (...) {
		LogPrintErr("Unknown exception in GetExposureTime");
		return false;
	}
}
API_CCD_Moudle_H bool GetPixelNum(int* size)
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
			LogPrintInfo("Pixel number: " + std::to_string(*size));
		}
		else {
			LogPrintErr("Failed to get pixel number");
		}

		return result;

	}
	catch (const std::exception& e) {
		LogPrintErr("Exception in GetPixelNum: " + std::string(e.what()));
		return false;
	}
	catch (...) {
		LogPrintErr("Unknown exception in GetPixelNum");
		return false;
	}
}

API_CCD_Moudle_H bool SetGain(int gain)
{
	std::lock_guard<std::mutex> lock(GlobalShare::g_mutex);
	return false;
}

API_CCD_Moudle_H bool GetGain(int * gain)
{
	std::lock_guard<std::mutex> lock(GlobalShare::g_mutex);
	return false;
}

API_CCD_Moudle_H bool EnableCooling(bool enable)
{
	std::lock_guard<std::mutex> lock(GlobalShare::g_mutex);
	return false;
}

API_CCD_Moudle_H bool SetCoolingTemperature(double temp)
{
	std::lock_guard<std::mutex> lock(GlobalShare::g_mutex);
	return false;
}

API_CCD_Moudle_H bool GetCurrentTemperature(double * temp)
{
	std::lock_guard<std::mutex> lock(GlobalShare::g_mutex);
	return false;
}

API_CCD_Moudle_H bool DataAcqOneShot(double * pd, int nPixSize)
{
	std::lock_guard<std::mutex> lock(GlobalShare::g_mutex);
	return false;
}

API_CCD_Moudle_H bool DataAcqOneShotImg(double * pdImg, int * nPixSize)
{
	std::lock_guard<std::mutex> lock(GlobalShare::g_mutex);
	return false;
}

API_CCD_Moudle_H bool GetSDKVersion(char * version)
{
	std::lock_guard<std::mutex> lock(GlobalShare::g_mutex);
	if (!version) {
		LogPrintErr("Null pointer for version");
		return false;
	}
	strncpy_s(version, strlen(MODULE_VERSION) + 1, MODULE_VERSION, strlen(MODULE_VERSION));
	LogPrintInfo("Retrieved SDK version: " + std::string(MODULE_VERSION));
	return true;
}

API_CCD_Moudle_H bool SDKCleanup()
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
		LogPrintErr("Exception in UnInitDll: " + std::string(e.what()));
		return false;
	}
	catch (...) {
		LogPrintErr("Unknown exception in UnInitDll");
		return false;
	}
}

API_CCD_Moudle_H const char* GetErrorMessage(SDKErrorCode errorCode) {
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

API_CCD_Moudle_H SDKErrorCode GetSdkLastError()
{
	return GlobalShare::g_s_lastError;
}

API_CCD_Moudle_H const char * GetLastErrorMessage()
{
	return GlobalShare::g_s_lastErrorMessage;
}

API_CCD_Moudle_H void ClearError()
{
	GlobalShare::g_s_lastError = SDK_SUCCESS;
	GlobalShare::g_s_lastErrorMessage[0] = '\0';
}


