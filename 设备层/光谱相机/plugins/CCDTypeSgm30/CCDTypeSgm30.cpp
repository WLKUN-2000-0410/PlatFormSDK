// CCDTypeSgm30.cpp : 定义 DLL 应用程序的导出函数。
//
// =============================================================================
// CCDTypeA 实现
// =============================================================================
#include "CCDTypeSgm30.h"


CCDTypeSgm30::CCDTypeSgm30(const std::string& deviceId)
	: m_deviceId(deviceId)
	, _opened(false)
	, m_exposureTime(100.0)
	, m_shouldStop(false)
	, _handle(nullptr)
{
}

CCDTypeSgm30::~CCDTypeSgm30() {

}


bool CCDTypeSgm30::Connect()
{	
	unsigned int status, device_num, i;
	//get compatible VID and PID
	unsigned int buffersize = 2;
	unsigned int VIDPID[4];
	//VIDPID = new unsigned int [4];
	VIDPID[0] = 4292;
	VIDPID[1] = 60001;
	VIDPID[2] = 1592;
	VIDPID[3] = 2732;

	_handle = nullptr;
	_opened = false;
	//get all device numbers
	for (unsigned int j = 0; j<buffersize * 2; j = j + 2)
	{
		device_num = 0;
		status = UAI_SpectrometerGetDeviceAmount(VIDPID[j], VIDPID[j + 1], &device_num);
		if (status == 0 && device_num > 0) // API_SUCCESS == status
		{
			for (i = 0; i<device_num; i++)
			{
				status = UAI_SpectrometerOpen(i, &_handle, VIDPID[j], VIDPID[j + 1]);
				if (status == 0)
				{
					//pixels
					UAI_SpectromoduleGetFrameSizeRaw(_handle, &_pixels);
					_binning_x = 2;

				//	get_exposure(&_exposure);
					_average = 1;
					_opened = true;
					return true;
				}
			}
		}
	}

	return false;
}

bool CCDTypeSgm30::DisConnect()
{
	auto ret = UAI_SpectrometerSetExternalPort(_handle, 0);
	if (API_SUCCESS != ret) return ret;

	ret = UAI_SpectrometerClose(_handle);
	if (API_SUCCESS != ret) return ret;

	_opened = false;
	return API_SUCCESS;
}

bool CCDTypeSgm30::SetExposureTime(double timeMs) {
	if (!_opened) return false;

	if (TypeA_SetExposure(timeMs)) {
		m_exposureTime = timeMs;
		return true;
	}
	return false;
}


double CCDTypeSgm30::GetExposureTime() const {
	return m_exposureTime;
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