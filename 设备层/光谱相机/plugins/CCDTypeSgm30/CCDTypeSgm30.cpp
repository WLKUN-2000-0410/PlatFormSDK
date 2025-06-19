// CCDTypeSgm30.cpp : 定义 DLL 应用程序的导出函数。
//

// =============================================================================
// CCD Type A 插件实现示例
// 文件: CCDTypeA.cpp (编译为 CCDTypeA.dll)
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
class CCDTypeA : public ICCDDevice {
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
	bool TypeA_Initialize(const std::string& deviceId);
	bool TypeA_Finalize();
	bool TypeA_SetExposure(double timeMs);
	bool TypeA_SetGain(double gain);
	bool TypeA_SetROI(int x, int y, int w, int h);
	bool TypeA_StartAcquisition();
	bool TypeA_StopAcquisition();
	bool TypeA_GetImage(void* buffer, size_t size);
	void TypeA_GetDeviceInfo(std::string& info, std::string& serial);

	// 内部方法
	void CaptureThreadFunc();

public:
	explicit CCDTypeA(const std::string& deviceId);
	virtual ~CCDTypeA();

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

// =============================================================================
// CCDTypeA 实现
// =============================================================================

CCDTypeA::CCDTypeA(const std::string& deviceId)
	: m_deviceId(deviceId)
	, m_connected(false)
	, m_exposureTime(100.0)
	, m_gain(1.0)
	, m_temperature(-10.0)
	, m_shouldStop(false)
{
}

CCDTypeA::~CCDTypeA() {

}


bool CCDTypeA::Connect()
{
	return false;
}

bool CCDTypeA::DisConnect()
{
	return false;
}

bool CCDTypeA::SetExposureTime(double timeMs) {
	if (!m_connected) return false;

	if (TypeA_SetExposure(timeMs)) {
		m_exposureTime = timeMs;
		return true;
	}
	return false;
}

bool CCDTypeA::SetGain(double gain) {
	if (!m_connected) return false;

	if (TypeA_SetGain(gain)) {
		m_gain = gain;
		return true;
	}
	return false;
}

bool CCDTypeA::SetTemperature(double temperature) {
	if (!m_connected) return false;

	// TypeA特定的温度控制逻辑
	m_temperature = temperature;
	return true;
}

double CCDTypeA::GetExposureTime() const {
	return m_exposureTime;
}

double CCDTypeA::GetGain() const {
	return m_gain;
}

bool CCDTypeA::setAB(const float & p)
{
	return false;
}

void CCDTypeA::CaptureThreadFunc() {
	//const size_t imageSize = m_roiWidth * m_roiHeight * 2; // 假设16位图像
	//std::vector<uint8_t> imageBuffer(imageSize);

	//while (!m_shouldStop) {
	//	if (TypeA_GetImage(imageBuffer.data(), imageSize)) {
	//		if (m_imageCallback) {
	//			m_imageCallback(imageBuffer.data(), imageSize, m_roiWidth, m_roiHeight);
	//		}
	//	}

	//	// 根据曝光时间等待
	//	std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(m_exposureTime)));
	//}
}

// =============================================================================
// 模拟TypeA SDK函数实现
// =============================================================================

bool CCDTypeA::TypeA_Initialize(const std::string& deviceId) {
	// 模拟初始化过程
	std::cout << "TypeA: Initializing device " << deviceId << std::endl;
	return true;
}

bool CCDTypeA::TypeA_Finalize() {
	std::cout << "TypeA: Finalizing device" << std::endl;
	return true;
}

bool CCDTypeA::TypeA_SetExposure(double timeMs) {
	std::cout << "TypeA: Setting exposure time to " << timeMs << " ms" << std::endl;
	return true;
}

bool CCDTypeA::TypeA_SetGain(double gain) {
	std::cout << "TypeA: Setting gain to " << gain << std::endl;
	return true;
}

bool CCDTypeA::TypeA_SetROI(int x, int y, int w, int h) {
	std::cout << "TypeA: Setting ROI to (" << x << "," << y << "," << w << "," << h << ")" << std::endl;
	return true;
}

bool CCDTypeA::TypeA_StartAcquisition() {
	std::cout << "TypeA: Starting acquisition" << std::endl;
	return true;
}

bool CCDTypeA::TypeA_StopAcquisition() {
	std::cout << "TypeA: Stopping acquisition" << std::endl;
	return true;
}

bool CCDTypeA::TypeA_GetImage(void* buffer, size_t size) {
	// 模拟图像数据获取
	if (buffer && size > 0) {
		// 生成模拟图像数据
		memset(buffer, rand() % 256, size);
		return true;
	}
	return false;
}

void CCDTypeA::TypeA_GetDeviceInfo(std::string& info, std::string& serial) {
	info = "TypeA CCD Camera";
	serial = "TA" + m_deviceId + "001";
}

// =============================================================================
// TypeA 工厂类实现
// =============================================================================

class CCDTypeAFactory : public ICCDFactory {
public:
	std::string GetDeviceType() const override {
		return "sgm30";
	}

	std::vector<std::string> EnumerateDevices() override {
		// 模拟设备枚举
		std::vector<std::string> devices;
		devices.push_back("Device001");
		devices.push_back("Device002");
		return devices;
	}

	std::unique_ptr<ICCDDevice> CreateDevice(const std::string& deviceId) override {
		return std::make_unique<CCDTypeA>(deviceId);
	}

	bool IsDeviceSupported(const std::string& deviceId) override {
		// 检查设备ID格式或通过SDK查询
		return deviceId.find("Device") != std::string::npos;
	}
};

// =============================================================================
// DLL导出函数实现
// =============================================================================

static CCDTypeAFactory* g_factory = nullptr;

extern "C" __declspec(dllexport) ICCDFactory* CreateFactory() {
	if (!g_factory) {
		g_factory = new CCDTypeAFactory();
	}
	return g_factory;
}

extern "C" __declspec(dllexport) void DestroyFactory(ICCDFactory* factory) {
	if (factory == g_factory) {
		delete g_factory;
		g_factory = nullptr;
	}
}

extern "C" __declspec(dllexport) const char* GetPluginInfo() {
	return "sgm30 CCD Plugin v1.0 - Support for sgm30 series cameras";
}

extern "C" __declspec(dllexport) int GetPluginVersion() {
	return 100; // 版本1.0.0
}