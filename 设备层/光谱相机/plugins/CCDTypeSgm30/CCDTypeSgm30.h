#pragma once
// =============================================================================
// CCD Type Sgm30 插件实现示例
// 文件: CCDTypeSgm30.h (编译为 CCDTypeSgm30.dll)
// =============================================================================
#include "CCDModule.h" // 包含上面的接口定义
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace CCDModule;
// =============================================================================
// TypeA CCD设备实现
// =============================================================================
class CCDTypeSgm30 : public ICCDDevice 
{
private:
	std::string m_deviceId;
	std::string m_serialNumber;
	bool m_connected;

	// 设备参数
	double m_exposureTime;
	double m_gain;
	double m_temperature;

	// 采集线程相关
	std::thread m_captureThread;
	std::mutex m_captureMutex;
	std::condition_variable m_captureCV;
	bool m_shouldStop;

	// 模拟的TypeA SDK函数（实际应该调用真实的SDK）
	bool TypeA_SetExposure(double timeMs);

	// 内部方法
	void CaptureThreadFunc();

public:
	explicit CCDTypeSgm30(const std::string& deviceId);
	virtual ~CCDTypeSgm30();

	// ICCDDevice接口实现
	bool   Connect();
	bool   DisConnect();
	bool   SetExposureTime(double timeMs) override;
	bool   SetGain(double gain) override;
	bool   SetTemperature(double temperature) override;

	double GetExposureTime() const override;
	double GetGain() const override;

	bool  setAB(const float &p);
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