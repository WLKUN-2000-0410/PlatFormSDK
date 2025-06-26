// CCDTypeSgm30.cpp : 定义 DLL 应用程序的导出函数。
//
// =============================================================================
// CCDTypeA 实现
// =============================================================================
#include "CCDTypeDFiled.h"


CCDTypeDFiled::CCDTypeDFiled(const std::string& deviceId)
	: m_deviceId(deviceId)
	, _opened(false)
	, m_exposureTime(100.0)
	, m_shouldStop(false)
	, _handle(nullptr)
{
}

CCDTypeDFiled::~CCDTypeDFiled() {

}


bool CCDTypeDFiled::Connect()
{

	return false;
}

bool CCDTypeDFiled::DisConnect()
{
	return false;
}

bool CCDTypeDFiled::SetExposureTime(double timeMs) {
	if (!_opened) return false;

	if (TypeA_SetExposure(timeMs)) {
		m_exposureTime = timeMs;
		return true;
	}
	return false;
}


double CCDTypeDFiled::GetExposureTime() const {
	return m_exposureTime;
}

bool CCDTypeDFiled::SetCoolingTemperature(double temp)
{
	return false;
}

double CCDTypeDFiled::GetCurrentTemperature()
{
	return 0.0;
}

bool CCDTypeDFiled::EnableCooling(bool enable)
{
	return false;
}

bool CCDTypeDFiled::SetGain(int gain)
{
	return false;
}

int CCDTypeDFiled::GetGain()
{
	return 0;
}

void CCDTypeDFiled::CaptureThreadFunc() {
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


bool CCDTypeDFiled::TypeA_SetExposure(double timeMs) {
	std::cout << "TypeA: Setting exposure time to " << timeMs << " ms" << std::endl;
	return true;
}

// =============================================================================
// TypeA 工厂类实现
// =============================================================================


std::string CCDTypeDFiledFactory::GetDeviceType() const
{
	return "DFiled";
}

std::vector<std::string> CCDTypeDFiledFactory::EnumerateDevices()
{
	// 模拟设备枚举
	std::vector<std::string> devices;
	devices.push_back("Device001");
	return devices;
}

std::unique_ptr<ICCDDevice> CCDTypeDFiledFactory::CreateDevice(const std::string& deviceId)
{
	return std::make_unique<CCDTypeDFiled>(deviceId);
}

bool CCDTypeDFiledFactory::IsDeviceSupported(const std::string& deviceId)
{
	// 检查设备ID格式或通过SDK查询
	return deviceId.find("Device") != std::string::npos;
}


// =============================================================================
// DLL导出函数实现
// =============================================================================

static CCDTypeDFiledFactory* g_factory = nullptr;

extern "C" __declspec(dllexport) ICCDFactory* CreateFactory()
{
	if (!g_factory) {
		g_factory = new CCDTypeDFiledFactory();
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