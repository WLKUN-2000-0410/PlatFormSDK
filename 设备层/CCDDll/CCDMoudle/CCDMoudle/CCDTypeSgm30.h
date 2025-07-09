#pragma once
#include "CCDInterface.h"
#include "CCDTypes.h"
#include <Windows.h>


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
	std::vector<HMODULE> m_dlls; // 存储加载的DLL句柄
	double m_exposureTime; // 当前曝光时间
	double m_temperature;  // 当前温度
	int m_gain;           // 当前增益
	bool m_isConnected;   // 连接状态
};

