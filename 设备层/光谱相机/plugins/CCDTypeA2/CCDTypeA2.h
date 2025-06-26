#pragma once
// =============================================================================
// CCD Type A2 ���ʵ��ʾ��
// �ļ�: CCDTypeA2.h (����Ϊ CCDTypeA2.dll)
// =============================================================================
#include "CCDModule.h" // ��������Ľӿڶ���
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Dependency\ATMCD32D.H"

using namespace CCDModule;
// =============================================================================
// TypeA CCD�豸ʵ��
// =============================================================================
class CCDTypeA2 : public ICCDDevice, public ICoolingModule, public IGainModule
{
public:
	explicit CCDTypeA2(const std::string& deviceId);
	virtual ~CCDTypeA2();

	// ICCDDevice�ӿ�ʵ��
	bool   Connect()override;
	bool   DisConnect()override;
	bool   SetExposureTime(double timeMs) override;
	double GetExposureTime() const override;

	//ICoolingModule �ӿ�ʵ��
	bool   SetCoolingTemperature(double temp) override;
	double GetCurrentTemperature() override;
	bool   EnableCooling(bool enable) override;

	// ���湦�ܽӿ�
	bool   SetGain(int gain) override;
	int    GetGain() override;

	// �麯�� 
	bool HasCoolingModule() const { return true; }
	bool HasGainModule() const { return true; }

private:
	std::string m_deviceId;
	std::string m_serialNumber;

	// �豸����
	double m_exposureTime;

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

class CCDTypeA2Factory : public ICCDFactory
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