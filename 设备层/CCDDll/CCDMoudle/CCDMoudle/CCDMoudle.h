#pragma once
#define API_CCD_Moudle_H extern "C" __declspec(dllexport)


typedef enum {
	SDK_SUCCESS = 0,
	SDK_ERROR_INVALID_PARAM = -1,
	SDK_ERROR_MEMORY_ALLOC = -2,
	SDK_ERROR_NETWORK = -3,
	SDK_ERROR_TIMEOUT = -4,
	SDK_ERROR_DEVICE_NOT_FOUND = -5,
	SDK_ERROR_DEVICE_NOT_CONNECTED = -6,
	SDK_ERROR_DEVICE_BUSY = -7,
	SDK_ERROR_COOLING_FAILED = -8,
	SDK_ERROR_ACQUISITION_FAILED = -9,
	SDK_ERROR_INVALID_TEMPERATURE = -10,
	SDK_ERROR_INVALID_GAIN = -11,
	SDK_ERROR_INVALID_EXPOSURE_TIME = -12,
	SDK_ERROR_BUFFER_TOO_SMALL = -13,
	SDK_ERROR_UNKNOWN = -999
} SDKErrorCode;

extern "C"
{
	API_CCD_Moudle_H	bool			SDKInit();
	API_CCD_Moudle_H	bool			Connect();
	API_CCD_Moudle_H	bool			IsConnected();
	API_CCD_Moudle_H	bool			DisConnect();
	API_CCD_Moudle_H	bool			SetExposureTime(double timeMs);
	API_CCD_Moudle_H	bool			GetExposureTime(double* timeMs);
	API_CCD_Moudle_H	bool			GetPixelNum(int* size);
	API_CCD_Moudle_H	bool			SetGain(int gain);
	API_CCD_Moudle_H	bool			GetGain(int *gain);
	API_CCD_Moudle_H	bool			EnableCooling(bool enable);
	API_CCD_Moudle_H	bool			SetCoolingTemperature(double temp);
	API_CCD_Moudle_H	bool			GetCurrentTemperature(double* temp);
	API_CCD_Moudle_H	bool			DataAcqOneShot(double *pd, int  nPixSize);
	API_CCD_Moudle_H	bool			DataAcqOneShotImg(double *pdImg, int *nPixSize);
	API_CCD_Moudle_H	bool			GetSDKVersion(char* version); //Specify a 32-byte buffer
	API_CCD_Moudle_H	const char*     GetErrorMessage(SDKErrorCode errorCode);
	API_CCD_Moudle_H	SDKErrorCode    GetSdkLastError();
	API_CCD_Moudle_H	const char*     GetLastErrorMessage();
	API_CCD_Moudle_H	void            ClearError();
	API_CCD_Moudle_H	bool			SDKCleanup();




}