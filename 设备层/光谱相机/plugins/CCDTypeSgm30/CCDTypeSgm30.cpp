// CCDTypeSgm30.cpp : 定义 DLL 应用程序的导出函数。
//
// =============================================================================
// CCDTypeA 实现
// =============================================================================
#include "CCDTypeSgm30.h"

CCDTypeSgm30::CCDTypeSgm30(const std::string& deviceId)
	: m_deviceId(deviceId)
	, m_connected(false)
	, m_exposureTime(100.0)
	, m_gain(1.0)
	, m_temperature(-10.0)
	, m_shouldStop(false)
{
}

CCDTypeSgm30::~CCDTypeSgm30() {

}


bool CCDTypeSgm30::Connect()
{
	return false;
}

bool CCDTypeSgm30::DisConnect()
{
	return false;
}

bool CCDTypeSgm30::SetExposureTime(double timeMs) {
	if (!m_connected) return false;

	if (TypeA_SetExposure(timeMs)) {
		m_exposureTime = timeMs;
		return true;
	}
	return false;
}

bool CCDTypeSgm30::SetGain(double gain) {
	if (!m_connected) return false;

/*	if (TypeA_SetGain(gain)) {
		m_gain = gain;
		return true;
	}*/
	return false;
}

bool CCDTypeSgm30::SetTemperature(double temperature) {
	if (!m_connected) return false;

	// TypeA特定的温度控制逻辑
	m_temperature = temperature;
	return true;
}

double CCDTypeSgm30::GetExposureTime() const {
	return m_exposureTime;
}

double CCDTypeSgm30::GetGain() const {
	return m_gain;
}

bool CCDTypeSgm30::setAB(const float & p)
{
	return false;
}

void CCDTypeSgm30::CaptureThreadFunc() {
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


bool CCDTypeSgm30::TypeA_SetExposure(double timeMs) {
	std::cout << "TypeA: Setting exposure time to " << timeMs << " ms" << std::endl;
	return true;
}

// =============================================================================
// TypeA 工厂类实现
// =============================================================================


std::string CCDTypeSgm30Factory::GetDeviceType() const
{
	return "sgm30";
}

std::vector<std::string> CCDTypeSgm30Factory::EnumerateDevices()
{
	// 模拟设备枚举
	std::vector<std::string> devices;
	devices.push_back("Device001");
	devices.push_back("Device002");
	return devices;
}

std::unique_ptr<ICCDDevice> CCDTypeSgm30Factory::CreateDevice(const std::string& deviceId)
{
	return std::make_unique<CCDTypeSgm30>(deviceId);
}

bool CCDTypeSgm30Factory::IsDeviceSupported(const std::string& deviceId)
{
	// 检查设备ID格式或通过SDK查询
	return deviceId.find("Device") != std::string::npos;
}


// =============================================================================
// DLL导出函数实现
// =============================================================================

static CCDTypeSgm30Factory* g_factory = nullptr;

extern "C" __declspec(dllexport) ICCDFactory* CreateFactory() 
{
	if (!g_factory) {
		g_factory = new CCDTypeSgm30Factory();
	}
	return g_factory;
}

extern "C" __declspec(dllexport) void DestroyFactory(ICCDFactory* factory) 
{
	if (factory == g_factory) {
		delete g_factory;
		g_factory = nullptr;
	}
}

extern "C" __declspec(dllexport) const char* GetPluginInfo() 
{
	return "sgm30 CCD Plugin v1.0 - Support for sgm30 series cameras";
}

extern "C" __declspec(dllexport) int GetPluginVersion() 
{
	return 100; // 版本1.0.0
}