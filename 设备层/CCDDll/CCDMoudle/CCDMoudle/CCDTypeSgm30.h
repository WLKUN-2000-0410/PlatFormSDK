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
	std::vector<HMODULE> m_dlls; // �洢���ص�DLL���
	double m_exposureTime; // ��ǰ�ع�ʱ��
	double m_temperature;  // ��ǰ�¶�
	int m_gain;           // ��ǰ����
	bool m_isConnected;   // ����״̬
};

