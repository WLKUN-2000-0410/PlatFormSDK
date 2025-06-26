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
#include "Dependency\dfield.h"

using namespace CCDModule;
// =============================================================================
// TypeA CCD�豸ʵ��
// =============================================================================
class CCDTypeDFiled : public ICCDDevice,public ICoolingModule,public IGainModule
{
public:
	explicit CCDTypeDFiled(const std::string& deviceId);
	virtual ~CCDTypeDFiled();

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

class CCDTypeDFiledFactory : public ICCDFactory
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