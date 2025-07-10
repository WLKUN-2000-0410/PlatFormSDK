#pragma once
#include "CCDInterface.h"
#include "CCDTypes.h"
#include <Windows.h>
#include "DllInterface.h"


class CCDTypeSgm30 : public CCDBase
{
public:
	CCDTypeSgm30();
	~CCDTypeSgm30();
public:
	bool   Initialize(const CCDConfig& config);
	bool   IsConnected();
	bool   Connect();
	bool   DisConnect();
	bool   SetExposureTime(double timeMs);
	bool   GetExposureTime(double* timeMs);
	bool   GetPixelNum(int* size);


private:
	bool   initDll();
	bool   loadDllFun();
private:
	HINSTANCE	Handle;      
	double		m_exposureTime; 
	double		m_temperature;  
	int			m_gain;           
	bool		m_isConnected;   


	DLL_OutER_inUI1_inUI2_inUIp3				UAI_SpectrometerGetDeviceAmount = nullptr;
	DLL_OutER_inUI1_inVpp2_inUI3_inUI4			UAI_SpectrometerOpen = nullptr;
	DLL_OutER_inVp1_inUSp2						UAI_SpectromoduleGetFrameSizeRaw = nullptr;
	DLL_OutER_inVp1_inUI2						UAI_SpectrometerSetExternalPort = nullptr;
	DLL_OutER_inVp1								UAI_SpectrometerClose = nullptr;
	DLL_OutER_inVp1_inUI2						UAI_SpectrometerSetIntegrationTime = nullptr;
	DLL_OutER_inVp1_inUIp2						UAI_SpectrometerGetIntegrationTime = nullptr;
	DLL_OutER_inVp1_inUI2_infp3_inUI4			UAI_SpectrometerDataOneshotRaw = nullptr;
	DLL_OutER_inVp1_inUCp2_inUI3_inUI4			UAI_SpectrometerGetUserRom = nullptr;
	DLL_OutER_inVp1_inUCp2_inUI3_inUI4			UAI_SpectrometerSetUserRom = nullptr;
	DLL_OutER_inVp1_inDp2_inUS3_inUIp4_inDp5    UAI_SpectromoduleGetIntensityCalibration = nullptr;
	DLL_OutER_inVp1_inUSp2						UAI_SpectromoduleGetFrameSize = nullptr;
	DLL_OutER_inVp1_inDp2_inUS3_inUI4_inD5		UAI_SpectromoduleSetIntensityCalibration = nullptr;
	DLL_OutER_inVp1_inUI2						UAI_SpectrometerBlockModeStartStop = nullptr;
	DLL_OutER_inVp1_inUIp2						UAI_SpectrometerGetExternalPort = nullptr;
	DLL_OutER_inVp1_inUI2_inUI3_inUI4			UAI_SpectrometerSetTriggerIO = nullptr;
	DLL_OutER_inVp1_inUI2_inUI3_inFp4           DLI_SpectrometerCheckDoneAndGetTriggerData = nullptr;
	DLL_OutER_inVp1_inUI2_inUIp3				UAI_SpectrometerSetTriggerGroupIntegrationTime = nullptr;



};

