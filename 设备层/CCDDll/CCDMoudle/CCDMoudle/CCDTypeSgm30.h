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
	bool   DataAcqOneShot(unsigned short * buff, unsigned long size) override;

private:
	bool   initDll();
	void   loadDllFun();
private:
	HINSTANCE	Handle;      
	double		m_exposureTime; 
	double		m_temperature;  
	int			m_gain;           
	bool		m_isConnected;   


	void*           _handle;
	unsigned short  _binning_x;
	unsigned int    _average;
	unsigned int    _exposure;
	unsigned short  _pixels;


	DLL_OutER_inUI1_inUI2_inUIp3				UAI_SpectrometerGetDeviceAmount ;
	DLL_OutER_inUI1_inVpp2_inUI3_inUI4			UAI_SpectrometerOpen ;
	DLL_OutER_inVp1_inUSp2						UAI_SpectromoduleGetFrameSizeRaw ;
	DLL_OutER_inVp1_inUI2						UAI_SpectrometerSetExternalPort ;
	DLL_OutER_inVp1								UAI_SpectrometerClose ;
	DLL_OutER_inVp1_inUI2						UAI_SpectrometerSetIntegrationTime ;
	DLL_OutER_inVp1_inUIp2						UAI_SpectrometerGetIntegrationTime ;
	DLL_OutER_inVp1_inUI2_infp3_inUI4			UAI_SpectrometerDataOneshotRaw ;
	DLL_OutER_inVp1_inUCp2_inUI3_inUI4			UAI_SpectrometerGetUserRom ;
	DLL_OutER_inVp1_inUCp2_inUI3_inUI4			UAI_SpectrometerSetUserRom ;
	DLL_OutER_inVp1_inDp2_inUS3_inUIp4_inDp5    UAI_SpectromoduleGetIntensityCalibration ;
	DLL_OutER_inVp1_inUSp2						UAI_SpectromoduleGetFrameSize ;
	DLL_OutER_inVp1_inDp2_inUS3_inUI4_inD5		UAI_SpectromoduleSetIntensityCalibration ;
	DLL_OutER_inVp1_inUI2						UAI_SpectrometerBlockModeStartStop ;
	DLL_OutER_inVp1_inUIp2						UAI_SpectrometerGetExternalPort ;
	DLL_OutER_inVp1_inUI2_inUI3_inUI4			UAI_SpectrometerSetTriggerIO ;
	DLL_OutER_inVp1_inUI2_inUI3_inFp4           DLI_SpectrometerCheckDoneAndGetTriggerData ;
	DLL_OutER_inVp1_inUI2_inUIp3				UAI_SpectrometerSetTriggerGroupIntegrationTime ;



};

