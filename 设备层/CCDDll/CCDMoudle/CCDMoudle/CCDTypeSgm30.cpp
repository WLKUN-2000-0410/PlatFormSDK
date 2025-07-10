
#include "CCDTypeSgm30.h"
#include <fstream>
#include "CDConfigManager.h"


CCDTypeSgm30::CCDTypeSgm30() : m_exposureTime(100.0), m_temperature(20.5), m_gain(1), m_isConnected(false), Handle(nullptr)
{
	UAI_SpectrometerGetDeviceAmount = nullptr;
	UAI_SpectrometerOpen = nullptr;
	UAI_SpectromoduleGetFrameSizeRaw = nullptr;
	UAI_SpectrometerSetExternalPort = nullptr;
	UAI_SpectrometerClose = nullptr;
	UAI_SpectrometerSetIntegrationTime = nullptr;
	UAI_SpectrometerGetIntegrationTime = nullptr;
	UAI_SpectrometerDataOneshotRaw = nullptr;
	UAI_SpectrometerGetUserRom = nullptr;
	UAI_SpectrometerSetUserRom = nullptr;
	UAI_SpectromoduleGetIntensityCalibration = nullptr;
	UAI_SpectromoduleGetFrameSize = nullptr;
	UAI_SpectromoduleSetIntensityCalibration = nullptr;
	UAI_SpectrometerBlockModeStartStop = nullptr;
	UAI_SpectrometerGetExternalPort = nullptr;
	UAI_SpectrometerSetTriggerIO = nullptr;
	DLI_SpectrometerCheckDoneAndGetTriggerData = nullptr;
	UAI_SpectrometerSetTriggerGroupIntegrationTime = nullptr;
}


CCDTypeSgm30::~CCDTypeSgm30()
{
	// 释放所有加载的DLL
	LogPrintInfo("卸载模块相关库：SGM30 -> {0}", bool(FreeLibrary(Handle)));
}

bool CCDTypeSgm30::Initialize(const CCDConfig & config)
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
	unsigned int status, device_num, i;
	//get compatible VID and PID
	unsigned int buffersize = 2;
	unsigned int VIDPID[4];
	//VIDPID = new unsigned int [4];
	VIDPID[0] = 4292;
	VIDPID[1] = 60001;
	VIDPID[2] = 1592;
	VIDPID[3] = 2732;

	_handle = nullptr;

	//get all device numbers
	for (unsigned int j = 0; j<buffersize * 2; j = j + 2)
	{
		device_num = 0;
		status = UAI_SpectrometerGetDeviceAmount(VIDPID[j], VIDPID[j + 1], &device_num);
		if (status == 0 && device_num > 0) // API_SUCCESS == status
		{
			for (i = 0; i<device_num; i++)
			{
				status = UAI_SpectrometerOpen(i, &_handle, VIDPID[j], VIDPID[j + 1]);
				if (status == 0)
				{
					//pixels
					UAI_SpectromoduleGetFrameSizeRaw(_handle, &_pixels);
					_binning_x = 2;


					_average = 1;
					m_isConnected = true;

					return true;
				}
			}
		}
	}
	return false;
}

bool CCDTypeSgm30::DisConnect()
{
	if (!m_isConnected) {
		LogPrintInfo("SGM30 already disconnected");
		return true;
	}
	if (!_handle) {
		LogPrintErr("Invalid device handle");
		m_isConnected = false;
		return false;
	}

	int ret = UAI_SpectrometerSetExternalPort(_handle, 0);
	if (ret != API_SUCCESS) {
		LogPrintErr("UAI_SpectrometerSetExternalPort failed with code: {0}" ,ret);
		m_isConnected = false;
		return false;
	}

	ret = UAI_SpectrometerClose(_handle);
	if (ret != API_SUCCESS) {
		LogPrintErr("UAI_SpectrometerClose failed with code: {0}" ,ret);
		m_isConnected = false;
		return false;
	}
	m_isConnected = false;
	LogPrintInfo("SGM30 disconnected successfully");
	return true;
}

bool CCDTypeSgm30::SetExposureTime(double timeMs)
{
	if (!m_isConnected) return false;
	int us = timeMs*1e3;
	return (UAI_SpectrometerSetIntegrationTime(_handle, us) == API_SUCCESS);
}

bool CCDTypeSgm30::GetExposureTime(double * timeMs)
{
	if (!m_isConnected) return false;
	unsigned int us = 0;
	if (API_SUCCESS == UAI_SpectrometerGetIntegrationTime(_handle, &us))
	{
		_exposure = us;
		*timeMs = us / 1000.0;
		return true;
	}
	return false;
}

bool CCDTypeSgm30::GetPixelNum(int * size)
{
	if (!m_isConnected) return false;
	*size = _pixels;
	return true;
}

bool CCDTypeSgm30::initDll()
{
	auto strdll = "UserApplication.dll";
	Handle = LoadLibraryA(strdll);
	if (!Handle)
	{
		LogPrintErr("load dll {0} error", strdll);
		return false;
	}
	loadDllFun();
	return true;
}


void CCDTypeSgm30::loadDllFun()
{
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

//bool CCDTypeSgm30::GetAcquiredData(unsigned short * buff, unsigned long size)
//{
//	if (!_opened) return false;
//
//	if (!buff || size == 0 || !_handle)
//	{
//		return false;
//	}
//
//	std::vector<float> vbuffer(size, 0.0);
//	int ret = UAI_SpectrometerDataOneshotRaw(_handle, _exposure, vbuffer.data(), /*_average*/1);
//	if (ret == 0x80000005)
//	{
//		return false;
//	}
//	for (size_t i = 0; i < size; i++)
//	{
//		float value = vbuffer[i];
//
//		// 安全转换
//		if (value < 0.0f)
//		{
//			buff[i] = 0;
//		}
//		else if (value > 65535.0f)
//		{
//			buff[i] = 65535;
//		}
//		else
//		{
//			buff[i] = static_cast<unsigned short>(std::round(value));
//		}
//	}
//	return true;
//}