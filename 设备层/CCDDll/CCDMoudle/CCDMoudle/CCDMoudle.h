#pragma once
#define API_CCD_Moudle_H extern "C" __declspec(dllexport)

extern "C"
{
	API_CCD_Moudle_H  bool    InitDll();
	API_CCD_Moudle_H  bool    Connect();
	API_CCD_Moudle_H  bool    IsConnected();
	API_CCD_Moudle_H  bool    DisConnect();
	API_CCD_Moudle_H  bool    SetExposureTime(double timeMs);
	API_CCD_Moudle_H  bool    GetExposureTime(double* timeMs);
	API_CCD_Moudle_H  bool    GetPixelNum(int* size);
	API_CCD_Moudle_H  bool    SetGain(int gain);
	API_CCD_Moudle_H  bool    GetGain(int *gain);
	API_CCD_Moudle_H  bool    EnableCooling(bool enable);
	API_CCD_Moudle_H  bool    SetCoolingTemperature(double temp);
	API_CCD_Moudle_H  bool    GetCurrentTemperature(double* temp);
	API_CCD_Moudle_H  bool    DataAcqOneShot(double *pd, int  nPixSize);
	API_CCD_Moudle_H  bool    DataAcqOneShotImg(double *pdImg, int *nPixSize);
	API_CCD_Moudle_H  bool    GetSDKVersion(char* version);
	API_CCD_Moudle_H  bool    UnInitDll();
}