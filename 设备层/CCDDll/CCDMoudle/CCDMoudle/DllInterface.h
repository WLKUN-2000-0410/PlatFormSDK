#pragma once
#include <Windows.h>


typedef enum
{
	//Internal Definition
	API_INT_START = 0,
	API_SUCCESS = API_INT_START,
	API_INT_BUFFER_INVALID,
	API_INT_FEATURE_UNSUPPORTED,
	API_INT_PROTOCOL_ERROR,
	API_INT_CALIBRATION_ERROR,
	API_INT_MEMORY_ERROR,
	API_INT_ARGUMENT_ERROR,
	API_INT_HANDLE_INVALID,
	API_INT_TIMEOUT,
	//API_INT_DDI_BUFFER_ERROR,
	API_INT_DATA_NOT_READY,
	API_INT_DATA_TIME_OUT,
	API_INT_FILE_IO_ERROR, //20161012 Kevin
	API_INT_FILE_Exist_ERROR, //20170913 Kevin
	API_INT_END,

	//Vendor Reservation Code
	API_EXT_START = 0x80000000
} ERRORCODE;

//DLL_API ERRORCODE UAI_SpectrometerSetTriggerGroupIntegrationTime(void*, unsigned int, unsigned int*);

typedef ERRORCODE(*DLL_OutER_inVp1_inUI2_inUIp3)(void*, unsigned int, unsigned int*);
typedef ERRORCODE(*DLL_OutER_inVp1_inUI2_inUI3_inFp4)(void*, unsigned int, unsigned int, float*);
typedef ERRORCODE(*DLL_OutER_inVp1_inUI2_inUI3_inUI4)(void*, unsigned int, unsigned int, unsigned int);
typedef ERRORCODE(*DLL_OutER_inVp1_inDp2_inUS3_inUI4_inD5)(void*, double *, unsigned short, unsigned int, double);
typedef ERRORCODE(*DLL_OutER_inVp1_inDp2_inUS3_inUIp4_inDp5)(void*, double *, unsigned short *, unsigned int *, double *);
typedef ERRORCODE(*DLL_OutER_inVp1_inUCp2_inUI3_inUI4)(void*, unsigned char *, unsigned int, unsigned int);
typedef ERRORCODE(*DLL_OutER_inVp1_inUI2_infp3_inUI4)(void*, unsigned int, float *, unsigned int);
typedef ERRORCODE(*DLL_OutER_inVp1_inUIp2)(void*, unsigned int *);
typedef ERRORCODE(*DLL_OutER_inVp1)(void*);
typedef ERRORCODE(*DLL_OutER_inVp1_inUI2)(void*, unsigned int);
typedef ERRORCODE(*DLL_OutER_inVp1_inUSp2)(void*, unsigned short *);
typedef ERRORCODE(*DLL_OutER_inUI1_inVpp2_inUI3_inUI4)(unsigned int, void**, unsigned int, unsigned int);
typedef ERRORCODE(*DLL_OutER_inUI1_inUI2_inUIp3)(unsigned int, unsigned int, unsigned int*);
typedef bool(*DLL_OutB_inI1_inFp2_inFp3_inI4_inFp5_inFp6_inI7_inFp8_inFp9_inIp10)(int, float*, float*, int, float*, float*, int, float*, float*, int*);
typedef bool(*DLL_OutB_inI1_inF2_inI3_inI4_inF5_inF6_inF7_inI8_inF9_inFp10_inIp11)(int, float, int, int, float, float, float, int, float, float*, int*);
typedef int(*DLL_OutI_inIp)(int*);
typedef bool(*DLL_OutB_inStr)(char*);
typedef int(*DLL_OutI_inStr)(char*);
typedef bool(*DLL_OutB_inI1_inStr)(int&, const char*);
typedef bool(*DLL_OutB_inI1_inC2_inStr)(int, char, char*);
typedef bool(*DLL_OutB_inI1_inStr_inI3)(int, char*, int);
typedef bool(*DLL_OutB_inI1_inI2_inBp)(int, int, bool*);
typedef bool(*DLL_OutB_inI1_inI2_inIp)(int, int, int*);
typedef bool(*DLL_OutB_inI1_inI2_inB3)(int, int, bool);
typedef bool(*DLL_OutB_inI1_inI2_inI3)(int, int, int);
typedef bool(*DLL_OutB_inI1)(int);
typedef bool(*DLL_OutB_inL)(long);
typedef bool(*DLL_OutB_inLp)(long*);
typedef bool(*DLL_OutB_inV)();
typedef bool(*DLL_OutB_inI1_inI2)(int, int);
typedef bool(*DLL_OutB_inI1_inF2)(int, float);
typedef bool(*DLL_OutB_inI1_inFp)(int, float*);
typedef int(*DLL_OutI_inI1_inFp)(int, float*);
typedef int(*DLL_OutI_inFp_inI)(float*, int);
typedef bool(*DLL_OutB_inI1_inIp)(int, int*);
typedef bool(*DLL_OutB_inI1_inI2_inFp)(int, int, float*);
typedef bool(*DLL_OutI_inI1_inI2_inFp3_inFp4_inI5)(int, int, float*, float*, int&);
typedef bool(*DLL_OutB_inI1_inBp)(int, bool*);
typedef void(*DLL_OutV_inI1_inStr)(int, char*);
typedef bool(*DLL_OutB_inI1_inB2)(int, bool);
typedef int(*DLL_OutI_inV)();
typedef bool(*DLL_OutB_inV)();
typedef bool(*DLL_OutB_inB1)(bool);
typedef bool(*DLL_OutB_inI1_inB2)(int, bool);
typedef bool(*DLL_OutB_inIQ1)(int);
typedef bool(*DLL_OutB_inIQ1_inIQ2)(int&, int&);
typedef bool(*DLL_OutB_inIQ1_inIQ2_inIQ3)(int&, int&, int&);
typedef void(*DLL_OutV_inBytep_inIp)(unsigned char*, int*);
typedef void(*DLL_OutV_inFunDataAcqCamImg)(DLL_OutV_inBytep_inIp);

