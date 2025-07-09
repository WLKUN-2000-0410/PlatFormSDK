
#include "CCDTypeSgm30.h"
#include <fstream>
#include "CDConfigManager.h"


CCDTypeSgm30::CCDTypeSgm30() : m_exposureTime(100.0), m_temperature(20.5), m_gain(1), m_isConnected(false), Handle(nullptr)
{
}


CCDTypeSgm30::~CCDTypeSgm30()
{
	// 释放所有加载的DLL
	LogPrintInfo("卸载模块相关库：SGM30 -> {0}", bool(FreeLibrary(Handle)));
}

bool CCDTypeSgm30::Initialize(const CCDConfig & config)
{
	// 检查并加载依赖文件
	for (std::vector<std::string>::const_iterator it = config.Dependentfiles.begin();
		it != config.Dependentfiles.end(); ++it)
	{
		if (it->empty()) {
			continue; // 跳过空文件名
		}
		// 检查文件是否存在
		std::ifstream file(*it);
		if (!file.good()) {
			LogPrintErr("Dependency file not found: {0}", *it);
			return false;
		}
		file.close();
		//// 加载DLL
		//HMODULE hDll = LoadLibraryA(it->c_str());
		//if (!hDll) {
		//	LogPrintErr("Failed to load DLL: ", *it);
		//	return false;
		//}
		//m_dlls.push_back(hDll);
	}
	if (initDll())
	{
		m_exposureTime = config.defaultExposureTime;
		m_temperature = config.defaulttemperature;
		m_gain = config.defaultgain;
		m_isConnected = false;
		return true;
	}
	return false;
}

bool CCDTypeSgm30::IsConnected()
{
	return m_isConnected;
}

bool CCDTypeSgm30::Connect()
{
	return false;
}

bool CCDTypeSgm30::DisConnect()
{
	return false;
}

bool CCDTypeSgm30::SetExposureTime(double timeMs)
{
	return false;
}

bool CCDTypeSgm30::GetExposureTime(double * timeMs)
{
	return false;
}

bool CCDTypeSgm30::GetPixelNum(int * size)
{
	return false;
}

bool CCDTypeSgm30::initDll()
{
	auto strdll = "UserApplication.dll";
	Handle = LoadLibraryA(strdll);
	if (!Handle)
	{
		LogPrintErr("load lib {0} error", strdll);
		return false;
	}
	loadDllFun();
	return true;
}

void CCDTypeSgm30::loadDllFun()
{
	UAI_SpectrometerGetDeviceAmount			= (DLL_OutER_inUI1_inUI2_inUIp3)GetProcAddress(Handle, "UAI_SpectrometerGetDeviceAmount");
	UAI_SpectrometerOpen					= (DLL_OutER_inUI1_inVpp2_inUI3_inUI4)GetProcAddress(Handle, "UAI_SpectrometerOpen");
	UAI_SpectromoduleGetFrameSizeRaw        = (DLL_OutER_inVp1_inUSp2)GetProcAddress(Handle, "UAI_SpectromoduleGetFrameSizeRaw");
	UAI_SpectrometerSetExternalPort         = (DLL_OutER_inVp1_inUI2)GetProcAddress(Handle, "UAI_SpectrometerSetExternalPort");
	UAI_SpectrometerClose                   = (DLL_OutER_inVp1)GetProcAddress(Handle, "UAI_SpectrometerClose");
	UAI_SpectrometerSetIntegrationTime      = (DLL_OutER_inVp1_inUI2)GetProcAddress(Handle, "UAI_SpectrometerSetIntegrationTime");
	UAI_SpectrometerGetIntegrationTime      = (DLL_OutER_inVp1_inUIp2)GetProcAddress(Handle, "UAI_SpectrometerGetIntegrationTime");
	UAI_SpectrometerDataOneshotRaw			= (DLL_OutER_inVp1_inUI2_infp3_inUI4)GetProcAddress(Handle, "UAI_SpectrometerDataOneshotRaw");
	UAI_SpectrometerGetUserRom              = (DLL_OutER_inVp1_inUCp2_inUI3_inUI4)GetProcAddress(Handle, "UAI_SpectrometerGetUserRom");
	UAI_SpectrometerSetUserRom              = (DLL_OutER_inVp1_inUCp2_inUI3_inUI4)GetProcAddress(Handle, "UAI_SpectrometerSetUserRom");
	UAI_SpectromoduleGetIntensityCalibration= (DLL_OutER_inVp1_inDp2_inUS3_inUIp4_inDp5)GetProcAddress(Handle, "UAI_SpectromoduleGetIntensityCalibration");
	UAI_SpectromoduleGetFrameSize			= (DLL_OutER_inVp1_inUSp2)GetProcAddress(Handle, "UAI_SpectromoduleGetFrameSize");
	UAI_SpectromoduleSetIntensityCalibration= (DLL_OutER_inVp1_inDp2_inUS3_inUI4_inD5)GetProcAddress(Handle, "UAI_SpectromoduleSetIntensityCalibration");
	UAI_SpectrometerBlockModeStartStop      = (DLL_OutER_inVp1_inUI2)GetProcAddress(Handle, "UAI_SpectrometerBlockModeStartStop");
	UAI_SpectrometerGetExternalPort			= (DLL_OutER_inVp1_inUIp2)GetProcAddress(Handle, "UAI_SpectrometerGetExternalPort");
	UAI_SpectrometerSetTriggerIO			= (DLL_OutER_inVp1_inUI2_inUI3_inUI4)GetProcAddress(Handle, "UAI_SpectrometerSetTriggerIO");



}
