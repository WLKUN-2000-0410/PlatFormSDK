#pragma once
// =============================================================================
// CCD Type Sgm30 ���ʵ��ʾ��
// �ļ�: CCDTypeSgm30.h (����Ϊ CCDTypeSgm30.dll)
// =============================================================================
#include "CCDModule.h" // ��������Ľӿڶ���
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Dependency\UserApplication.h"

using namespace CCDModule;
// =============================================================================
// TypeA CCD�豸ʵ��
// =============================================================================
class CCDTypeSgm30 : public ICCDDevice 
{
public:
	explicit CCDTypeSgm30(const std::string& deviceId);
	virtual ~CCDTypeSgm30();

	// ICCDDevice�ӿ�ʵ��
	bool   Connect();
	bool   DisConnect();
	bool   SetExposureTime(double timeMs) override;
	bool   SetGain(double gain) override;
	bool   SetTemperature(double temperature) override;

	double GetExposureTime() const override;
	double GetGain() const override;

private:
	std::string m_deviceId;
	std::string m_serialNumber;

	// �豸����
	double m_exposureTime;
	double m_gain;
	double m_temperature;

	// �ɼ��߳����
	std::thread m_captureThread;
	std::mutex m_captureMutex;
	std::condition_variable m_captureCV;
	bool m_shouldStop;

	// ģ���TypeA SDK������ʵ��Ӧ�õ�����ʵ��SDK��
	bool TypeA_SetExposure(double timeMs);

	// �ڲ�����
	void CaptureThreadFunc();
private:
	void*           _handle;
	unsigned short  _binning_x;
	unsigned int    _average;
	bool            _opened;
	unsigned int    _exposure;
	unsigned short  _pixels;
};

class CCDTypeSgm30Factory : public ICCDFactory
{
public:
	std::string GetDeviceType() const override;

	std::vector<std::string> EnumerateDevices() override;

	std::unique_ptr<ICCDDevice> CreateDevice(const std::string& deviceId) override;

	bool IsDeviceSupported(const std::string& deviceId) override;
};

extern "C" __declspec(dllexport) ICCDFactory* CreateFactory();
extern "C" __declspec(dllexport) void DestroyFactory(ICCDFactory* factory);
extern "C" __declspec(dllexport) const char* GetPluginInfo();
extern "C" __declspec(dllexport) int GetPluginVersion();