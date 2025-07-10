
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

bool CCDTypeSgm30::loadDllFun()
{
	if (!Handle) 
	{
		return false;
	}

	// 定义函数信息结构
	struct FunctionInfo {
		void** funcPtr;
		const char* funcName;
		const char* funcType;
	};

	// 函数列表
	FunctionInfo functions[] = {
		{ (void**)&UAI_SpectrometerGetDeviceAmount, "UAI_SpectrometerGetDeviceAmount", "DLL_OutER_inUI1_inUI2_inUIp3" },
		{ (void**)&UAI_SpectrometerOpen, "UAI_SpectrometerOpen", "DLL_OutER_inUI1_inVpp2_inUI3_inUI4" },
		{ (void**)&UAI_SpectromoduleGetFrameSizeRaw, "UAI_SpectromoduleGetFrameSizeRaw", "DLL_OutER_inVp1_inUSp2" },
		{ (void**)&UAI_SpectrometerSetExternalPort, "UAI_SpectrometerSetExternalPort", "DLL_OutER_inVp1_inUI2" },
		{ (void**)&UAI_SpectrometerClose, "UAI_SpectrometerClose", "DLL_OutER_inVp1" },
		{ (void**)&UAI_SpectrometerSetIntegrationTime, "UAI_SpectrometerSetIntegrationTime", "DLL_OutER_inVp1_inUI2" },
		{ (void**)&UAI_SpectrometerGetIntegrationTime, "UAI_SpectrometerGetIntegrationTime", "DLL_OutER_inVp1_inUIp2" },
		{ (void**)&UAI_SpectrometerDataOneshotRaw, "UAI_SpectrometerDataOneshotRaw", "DLL_OutER_inVp1_inUI2_infp3_inUI4" },
		{ (void**)&UAI_SpectrometerGetUserRom, "UAI_SpectrometerGetUserRom", "DLL_OutER_inVp1_inUCp2_inUI3_inUI4" },
		{ (void**)&UAI_SpectrometerSetUserRom, "UAI_SpectrometerSetUserRom", "DLL_OutER_inVp1_inUCp2_inUI3_inUI4" },
		{ (void**)&UAI_SpectromoduleGetIntensityCalibration, "UAI_SpectromoduleGetIntensityCalibration", "DLL_OutER_inVp1_inDp2_inUS3_inUIp4_inDp5" },
		{ (void**)&UAI_SpectromoduleGetFrameSize, "UAI_SpectromoduleGetFrameSize", "DLL_OutER_inVp1_inUSp2" },
		{ (void**)&UAI_SpectromoduleSetIntensityCalibration, "UAI_SpectromoduleSetIntensityCalibration", "DLL_OutER_inVp1_inDp2_inUS3_inUI4_inD5" },
		{ (void**)&UAI_SpectrometerBlockModeStartStop, "UAI_SpectrometerBlockModeStartStop", "DLL_OutER_inVp1_inUI2" },
		{ (void**)&UAI_SpectrometerGetExternalPort, "UAI_SpectrometerGetExternalPort", "DLL_OutER_inVp1_inUIp2" },
		{ (void**)&UAI_SpectrometerSetTriggerIO, "UAI_SpectrometerSetTriggerIO", "DLL_OutER_inVp1_inUI2_inUI3_inUI4" },
		{ (void**)&DLI_SpectrometerCheckDoneAndGetTriggerData, "DLI_SpectrometerCheckDoneAndGetTriggerData", "DLL_OutER_inVp1_inUI2_inUI3_inFp4" },
		{ (void**)&UAI_SpectrometerSetTriggerGroupIntegrationTime, "UAI_SpectrometerSetTriggerGroupIntegrationTime", "DLL_OutER_inVp1_inUI2_inUIp3" }
	};

	// 批量加载函数
	for (size_t i = 0; i < sizeof(functions) / sizeof(functions[0]); ++i) {
		*functions[i].funcPtr = GetProcAddress(Handle, functions[i].funcName);
		if (!*functions[i].funcPtr) {
			// 可以在这里添加详细的错误日志
			// printf("Failed to load function: %s (Type: %s)\n", functions[i].funcName, functions[i].funcType);
			return false;
		}
	}

	return true;
	UAI_SpectrometerGetDeviceAmount					= (DLL_OutER_inUI1_inUI2_inUIp3)GetProcAddress(Handle, "UAI_SpectrometerGetDeviceAmount");
	UAI_SpectrometerOpen							= (DLL_OutER_inUI1_inVpp2_inUI3_inUI4)GetProcAddress(Handle, "UAI_SpectrometerOpen");
	UAI_SpectromoduleGetFrameSizeRaw				= (DLL_OutER_inVp1_inUSp2)GetProcAddress(Handle, "UAI_SpectromoduleGetFrameSizeRaw");
	UAI_SpectrometerSetExternalPort					= (DLL_OutER_inVp1_inUI2)GetProcAddress(Handle, "UAI_SpectrometerSetExternalPort");
	UAI_SpectrometerClose							= (DLL_OutER_inVp1)GetProcAddress(Handle, "UAI_SpectrometerClose");
	UAI_SpectrometerSetIntegrationTime				= (DLL_OutER_inVp1_inUI2)GetProcAddress(Handle, "UAI_SpectrometerSetIntegrationTime");
	UAI_SpectrometerGetIntegrationTime				= (DLL_OutER_inVp1_inUIp2)GetProcAddress(Handle, "UAI_SpectrometerGetIntegrationTime");
	UAI_SpectrometerDataOneshotRaw					= (DLL_OutER_inVp1_inUI2_infp3_inUI4)GetProcAddress(Handle, "UAI_SpectrometerDataOneshotRaw");
	UAI_SpectrometerGetUserRom						= (DLL_OutER_inVp1_inUCp2_inUI3_inUI4)GetProcAddress(Handle, "UAI_SpectrometerGetUserRom");
	UAI_SpectrometerSetUserRom						= (DLL_OutER_inVp1_inUCp2_inUI3_inUI4)GetProcAddress(Handle, "UAI_SpectrometerSetUserRom");
	UAI_SpectromoduleGetIntensityCalibration		= (DLL_OutER_inVp1_inDp2_inUS3_inUIp4_inDp5)GetProcAddress(Handle, "UAI_SpectromoduleGetIntensityCalibration");
	UAI_SpectromoduleGetFrameSize					= (DLL_OutER_inVp1_inUSp2)GetProcAddress(Handle, "UAI_SpectromoduleGetFrameSize");
	UAI_SpectromoduleSetIntensityCalibration		= (DLL_OutER_inVp1_inDp2_inUS3_inUI4_inD5)GetProcAddress(Handle, "UAI_SpectromoduleSetIntensityCalibration");
	UAI_SpectrometerBlockModeStartStop				= (DLL_OutER_inVp1_inUI2)GetProcAddress(Handle, "UAI_SpectrometerBlockModeStartStop");
	UAI_SpectrometerGetExternalPort					= (DLL_OutER_inVp1_inUIp2)GetProcAddress(Handle, "UAI_SpectrometerGetExternalPort");
	UAI_SpectrometerSetTriggerIO					= (DLL_OutER_inVp1_inUI2_inUI3_inUI4)GetProcAddress(Handle, "UAI_SpectrometerSetTriggerIO");
	DLI_SpectrometerCheckDoneAndGetTriggerData      = (DLL_OutER_inVp1_inUI2_inUI3_inFp4)GetProcAddress(Handle, "DLI_SpectrometerCheckDoneAndGetTriggerData");
	UAI_SpectrometerSetTriggerGroupIntegrationTime  = (DLL_OutER_inVp1_inUI2_inUIp3)GetProcAddress(Handle, "UAI_SpectrometerSetTriggerGroupIntegrationTime");
}
