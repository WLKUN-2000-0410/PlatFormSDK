// CCDMoudle.cpp : ���� DLL Ӧ�ó���ĵ���������
//

// CCDMoudle.cpp : ���� DLL Ӧ�ó���ĵ���������
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


// �ڲ���������
namespace {
	// ��������ļ�������Ĭ������
	bool EnsureConfigFile() 
	{
		std::vector<std::string> configPaths = 
		{
			"./Ccd_config.ini",
			//"./config/config.ini",
			//"C:/CCDModule/config.ini"
		};

		auto& configMgr = CCDConfigManager::GetInstance();

		// ���Լ������������ļ�
		for (const auto& path : configPaths) 
		{
			if (configMgr.LoadConfig(path)) 
			{
				LogPrintInfo("Config loaded from: " + path);
				return true;
			}
		}

		// ���û���ҵ������ļ�������Ĭ������
		LogPrintInfo("No config file found, creating default config");

		// ����ȫ��Ĭ�ϲ���
		configMgr.SetGlobalParameter("DefaultCCDType", "SGM30");
		configMgr.SetGlobalParameter("LogLevel", "INFO");
		configMgr.SetGlobalParameter("TimeoutMs", "5000");

		// ���ø��豸Ĭ������
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

		// ����Ĭ������
		return configMgr.SaveConfig("./Ccd_config.ini");
	}
}
API_CCD_Moudle_H bool InitDll()
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


		// 1. ȷ�������ļ����ڲ�����
		if (!EnsureConfigFile()) {
			LogPrintErr("Failed to load or create config file");
			return false;
		}

		// 2. ��ȡ���ù�����
		auto& configMgr = CCDConfigManager::GetInstance();

		// 3. ��ȡĬ��CCD����
		std::string typeStr = configMgr.GetGlobalParameter("DefaultCCDType");
		if (typeStr.empty()) {
			typeStr = "SGM30"; // ���û�����ã�ʹ��Ĭ��ֵ
		}

		GlobalShare::g_currentType = StringToCCDType(typeStr);
		LogPrintInfo("Selected CCD type: " + typeStr);

		// 4. ����CCD�豸ʵ��
		GlobalShare::g_currentDevice = CCDFactory::CreateDevice(GlobalShare::g_currentType);
		if (!GlobalShare::g_currentDevice) {
			LogPrintErr("Failed to create CCD device of type: " + typeStr);
			return false;
		}

		// 5. ��ȡ�豸���ò���ʼ��
		CCDConfig config = configMgr.GetDeviceConfig(GlobalShare::g_currentType);
		if (!GlobalShare::g_currentDevice->Initialize(config)) {
			LogPrintErr("Failed to initialize CCD device");
			GlobalShare::g_currentDevice.reset();
			return false;
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

API_CCD_Moudle_H bool UnInitDll()
{
	std::lock_guard<std::mutex> lock(GlobalShare::g_mutex);

	try {
		if (!GlobalShare::g_initialized) {
			return true; // �Ѿ��������
		}

		LogPrintInfo("Starting DLL cleanup");

		// �Ͽ��豸����
		if (GlobalShare::g_currentDevice && GlobalShare::g_currentDevice->IsConnected()) {
			GlobalShare::g_currentDevice->DisConnect();
		}

		// �����豸ʵ��
		GlobalShare::g_currentDevice.reset();

		// ���������
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


