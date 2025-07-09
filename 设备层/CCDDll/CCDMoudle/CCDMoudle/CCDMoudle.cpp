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
// 全局状态变量
static std::shared_ptr<CCDBase> g_currentDevice = nullptr;
static CCDType g_currentType = CCDType::VIRTUAL;
static bool g_initialized = false;
static std::mutex g_mutex; // 线程安全保护

// 错误信息记录
static std::string g_lastError = "";
// 内部辅助函数
namespace {
	// 记录错误信息
	void SetLastError(const std::string& error) 
	{
		g_lastError = error;
		LogPrintErr("{0}", error);
	}

	// 记录信息日志
	void LogInfo(const std::string& info) 
	{
		LogPrintInfo("{0}", info);
	}

	// 检查配置文件并创建默认配置
	bool EnsureConfigFile() 
	{
		std::vector<std::string> configPaths = 
		{
			"./Ccd_config.ini",
			//"./config/config.ini",
			//"C:/CCDModule/config.ini"
		};

		auto& configMgr = CCDConfigManager::GetInstance();

		// 尝试加载现有配置文件
		for (const auto& path : configPaths) 
		{
			if (configMgr.LoadConfig(path)) 
			{
				LogInfo("Config loaded from: " + path);
				return true;
			}
		}

		// 如果没有找到配置文件，创建默认配置
		LogInfo("No config file found, creating default config");

		// 设置全局默认参数
		configMgr.SetGlobalParameter("DefaultCCDType", "SGM30");
		configMgr.SetGlobalParameter("LogLevel", "INFO");
		configMgr.SetGlobalParameter("TimeoutMs", "5000");

		// 设置各设备默认配置
		CCDConfig sgm30Config(CCDType::SGM30);
		sgm30Config.deviceName = "SGM30_Device";
		sgm30Config.port = "COM3";
		sgm30Config.baudRate = 115200;
		sgm30Config.defaultExposureTime = 100.0;
		sgm30Config.pixelNum = 2048;
		configMgr.SetDeviceConfig(CCDType::SGM30, sgm30Config);

		CCDConfig c2Config(CCDType::C2);
		c2Config.deviceName = "C2_Device";
		c2Config.port = "COM4";
		c2Config.baudRate = 9600;
		c2Config.defaultExposureTime = 50.0;
		c2Config.pixelNum = 1024;
		configMgr.SetDeviceConfig(CCDType::C2, c2Config);

		// 保存默认配置
		return configMgr.SaveConfig("./Ccd_config.ini");
	}
}
API_CCD_Moudle_H bool InitDll()
{
	std::lock_guard<std::mutex> lock(g_mutex);
	try 
	{
		if (g_initialized)
		{
			SetLastError("DLL already initialized");
			return false;
		}
		LogInfo("Starting DLL initialization");


		// 1. 确保配置文件存在并加载
		if (!EnsureConfigFile()) {
			SetLastError("Failed to load or create config file");
			return false;
		}

		// 2. 获取配置管理器
		auto& configMgr = CCDConfigManager::GetInstance();

		// 3. 获取默认CCD类型
		std::string typeStr = configMgr.GetGlobalParameter("DefaultCCDType");
		if (typeStr.empty()) {
			typeStr = "SGM30"; // 如果没有配置，使用默认值
		}

		g_currentType = StringToCCDType(typeStr);
		LogInfo("Selected CCD type: " + typeStr);

		// 4. 创建CCD设备实例
		g_currentDevice = CCDFactory::CreateDevice(g_currentType);
		if (!g_currentDevice) {
			SetLastError("Failed to create CCD device of type: " + typeStr);
			return false;
		}

		// 5. 获取设备配置并初始化
		CCDConfig config = configMgr.GetDeviceConfig(g_currentType);
		if (!g_currentDevice->Initialize(config)) {
			SetLastError("Failed to initialize CCD device");
			g_currentDevice.reset();
			return false;
		}

		g_initialized = true;
		LogInfo("DLL initialization completed successfully");
		return true;
	}
	catch (const std::exception& e) {
		SetLastError("Exception in InitDll: " + std::string(e.what()));
		return false;
	}
	catch (...) {
		SetLastError("Unknown exception in InitDll");
		return false;
	}
}

API_CCD_Moudle_H bool Connect()
{
	std::lock_guard<std::mutex> lock(g_mutex);

	try {
		if (!g_initialized) {
			SetLastError("DLL not initialized");
			return false;
		}

		if (!g_currentDevice) {
			SetLastError("No CCD device available");
			return false;
		}

		if (g_currentDevice->IsConnected()) {
			LogInfo("Device already connected");
			return true;
		}

		LogInfo("Attempting to connect to CCD device");
		bool result = g_currentDevice->Connect();

		if (result) {
			LogInfo("Successfully connected to CCD device");
		}
		else {
			SetLastError("Failed to connect to CCD device");
		}

		return result;

	}
	catch (const std::exception& e) {
		SetLastError("Exception in Connect: " + std::string(e.what()));
		return false;
	}
	catch (...) {
		SetLastError("Unknown exception in Connect");
		return false;
	}
}
API_CCD_Moudle_H bool IsConnected()
{
	std::lock_guard<std::mutex> lock(g_mutex);

	if (!g_initialized || !g_currentDevice) 
	{
		return false;
	}
	return g_currentDevice->IsConnected();
}
API_CCD_Moudle_H bool DisConnect()
{
	std::lock_guard<std::mutex> lock(g_mutex);

	try 
	{
		if (!g_initialized) {
			SetLastError("DLL not initialized");
			return false;
		}

		if (!g_currentDevice) {
			SetLastError("No CCD device available");
			return false;
		}

		if (!g_currentDevice->IsConnected()) {
			LogInfo("Device already disconnected");
			return true;
		}

		LogInfo("Attempting to disconnect from CCD device");
		bool result = g_currentDevice->DisConnect();

		if (result) {
			LogInfo("Successfully disconnected from CCD device");
		}
		else {
			SetLastError("Failed to disconnect from CCD device");
		}

		return result;

	}
	catch (const std::exception& e) {
		SetLastError("Exception in DisConnect: " + std::string(e.what()));
		return false;
	}
	catch (...) {
		SetLastError("Unknown exception in DisConnect");
		return false;
	}
}
API_CCD_Moudle_H bool SetExposureTime(double timeMs)
{
	std::lock_guard<std::mutex> lock(g_mutex);

	try {
		if (!g_initialized) {
			SetLastError("DLL not initialized");
			return false;
		}

		if (!g_currentDevice) {
			SetLastError("No CCD device available");
			return false;
		}

		if (!g_currentDevice->IsConnected()) {
			SetLastError("Device not connected");
			return false;
		}

		if (timeMs <= 0) {
			SetLastError("Invalid exposure time: " + std::to_string(timeMs));
			return false;
		}

		LogInfo("Setting exposure time to: " + std::to_string(timeMs) + " ms");
		bool result = g_currentDevice->SetExposureTime(timeMs);

		if (result) {
			LogInfo("Successfully set exposure time");
		}
		else {
			SetLastError("Failed to set exposure time");
		}

		return result;

	}
	catch (const std::exception& e) {
		SetLastError("Exception in SetExposureTime: " + std::string(e.what()));
		return false;
	}
	catch (...) {
		SetLastError("Unknown exception in SetExposureTime");
		return false;
	}
}
API_CCD_Moudle_H bool GetExposureTime(double* timeMs)
{
	std::lock_guard<std::mutex> lock(g_mutex);

	try {
		if (!timeMs) {
			SetLastError("Invalid parameter: timeMs is null");
			return false;
		}

		if (!g_initialized) {
			SetLastError("DLL not initialized");
			return false;
		}

		if (!g_currentDevice) {
			SetLastError("No CCD device available");
			return false;
		}

		if (!g_currentDevice->IsConnected()) {
			SetLastError("Device not connected");
			return false;
		}

		bool result = g_currentDevice->GetExposureTime(timeMs);

		if (result) {
			LogInfo("Current exposure time: " + std::to_string(*timeMs) + " ms");
		}
		else {
			SetLastError("Failed to get exposure time");
		}

		return result;

	}
	catch (const std::exception& e) {
		SetLastError("Exception in GetExposureTime: " + std::string(e.what()));
		return false;
	}
	catch (...) {
		SetLastError("Unknown exception in GetExposureTime");
		return false;
	}
}
API_CCD_Moudle_H bool GetPixelNum(int* size)
{
	std::lock_guard<std::mutex> lock(g_mutex);

	try {
		if (!size) {
			SetLastError("Invalid parameter: size is null");
			return false;
		}

		if (!g_initialized) {
			SetLastError("DLL not initialized");
			return false;
		}

		if (!g_currentDevice) {
			SetLastError("No CCD device available");
			return false;
		}

		bool result = g_currentDevice->GetPixelNum(size);

		if (result) {
			LogInfo("Pixel number: " + std::to_string(*size));
		}
		else {
			SetLastError("Failed to get pixel number");
		}

		return result;

	}
	catch (const std::exception& e) {
		SetLastError("Exception in GetPixelNum: " + std::string(e.what()));
		return false;
	}
	catch (...) {
		SetLastError("Unknown exception in GetPixelNum");
		return false;
	}
}

API_CCD_Moudle_H bool SetGain(int gain)
{
	std::lock_guard<std::mutex> lock(g_mutex);
	return false;
}

API_CCD_Moudle_H bool GetGain(int * gain)
{
	std::lock_guard<std::mutex> lock(g_mutex);
	return false;
}

API_CCD_Moudle_H bool EnableCooling(bool enable)
{
	std::lock_guard<std::mutex> lock(g_mutex);
	return false;
}

API_CCD_Moudle_H bool SetCoolingTemperature(double temp)
{
	std::lock_guard<std::mutex> lock(g_mutex);
	return false;
}

API_CCD_Moudle_H bool GetCurrentTemperature(double * temp)
{
	std::lock_guard<std::mutex> lock(g_mutex);
	return false;
}

API_CCD_Moudle_H bool DataAcqOneShot(double * pd, int nPixSize)
{
	std::lock_guard<std::mutex> lock(g_mutex);
	return false;
}

API_CCD_Moudle_H bool DataAcqOneShotImg(double * pdImg, int * nPixSize)
{
	std::lock_guard<std::mutex> lock(g_mutex);
	return false;
}

API_CCD_Moudle_H bool UnInitDll()
{
	std::lock_guard<std::mutex> lock(g_mutex);

	try {
		if (!g_initialized) {
			return true; // 已经清理过了
		}

		LogInfo("Starting DLL cleanup");

		// 断开设备连接
		if (g_currentDevice && g_currentDevice->IsConnected()) {
			g_currentDevice->DisConnect();
		}

		// 清理设备实例
		g_currentDevice.reset();

		// 清理管理器
		CCDManager::GetInstance().ClearAllDevices();

		g_initialized = false;
		g_currentType = CCDType::VIRTUAL;
		g_lastError.clear();

		LogInfo("DLL cleanup completed");
		return true;

	}
	catch (const std::exception& e) {
		SetLastError("Exception in UnInitDll: " + std::string(e.what()));
		return false;
	}
	catch (...) {
		SetLastError("Unknown exception in UnInitDll");
		return false;
	}
}


