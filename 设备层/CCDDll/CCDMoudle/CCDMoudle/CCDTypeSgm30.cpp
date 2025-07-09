
#include "CCDTypeSgm30.h"
#include <fstream>
#include "CDConfigManager.h"


CCDTypeSgm30::CCDTypeSgm30() : m_exposureTime(100.0), m_temperature(20.5), m_gain(1), m_isConnected(false)
{
}


CCDTypeSgm30::~CCDTypeSgm30()
{
	// �ͷ����м��ص�DLL
	for (std::vector<HMODULE>::iterator it = m_dlls.begin(); it != m_dlls.end(); ++it) {
		if (*it) {
			FreeLibrary(*it);
		}
	}
	m_dlls.clear();
}

bool CCDTypeSgm30::Initialize(const CCDConfig & config)
{
	// ��鲢���������ļ�
	for (std::vector<std::string>::const_iterator it = config.Dependentfiles.begin();
		it != config.Dependentfiles.end(); ++it)
	{
		if (it->empty()) {
			continue; // �������ļ���
		}
		// ����ļ��Ƿ����
		std::ifstream file(*it);
		if (!file.good()) {
			LogPrintErr("Dependency file not found: {0}", *it);
			return false;
		}
		file.close();
		// ����DLL
		HMODULE hDll = LoadLibraryA(it->c_str());
		if (!hDll) {
			LogPrintErr("Failed to load DLL: ", *it);
			return false;
		}
		m_dlls.push_back(hDll);
	}
	// ��ʼ���豸����
	m_exposureTime = config.defaultExposureTime;
	m_temperature = config.defaulttemperature;
	m_gain = config.defaultgain;
	m_isConnected = false;
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
