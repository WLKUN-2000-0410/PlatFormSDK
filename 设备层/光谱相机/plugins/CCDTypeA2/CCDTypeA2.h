#pragma once
// =============================================================================
// CCD Type A2 插件实现示例
// 文件: CCDTypeA2.h (编译为 CCDTypeA2.dll)
// =============================================================================
#include "CCDModule.h" // 包含上面的接口定义
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Dependency\ATMCD32D.H"

using namespace CCDModule;
// =============================================================================
// TypeA CCD设备实现
// =============================================================================
class CCDTypeA2 : public ICCDDevice, public ICoolingModule, public IGainModule
{
public:
	explicit CCDTypeA2(const std::string& deviceId);
	virtual ~CCDTypeA2();

	// ICCDDevice接口实现
	bool   Connect()override;
	bool   DisConnect()override;
	bool   SetExposureTime(double timeMs) override;
	double GetExposureTime() const override;

	//ICoolingModule 接口实现
	bool   SetCoolingTemperature(double temp) override;
	double GetCurrentTemperature() override;
	bool   EnableCooling(bool enable) override;

	// 增益功能接口
	bool   SetGain(int gain) override;
	int    GetGain() override;

	// 虚函数 
	bool HasCoolingModule() const { return true; }
	bool HasGainModule() const { return true; }

private:
	std::string m_deviceId;
	std::string m_serialNumber;

	// 设备参数
	double m_exposureTime;

	// 采集线程相关
	std::thread m_captureThread;
	std::mutex m_captureMutex;
	std::condition_variable m_captureCV;
	bool m_shouldStop;

	// 模拟的TypeA SDK函数（实际应该调用真实的SDK）
	bool TypeA_SetExposure(double timeMs);

	// 内部方法
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