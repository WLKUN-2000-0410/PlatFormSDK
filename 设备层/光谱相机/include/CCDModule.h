// =============================================================================
// 文件: include/CCDModule.h
// 描述: CCD模块统一架构的公共接口定义，包含设备接口、工厂接口和插件管理器
// =============================================================================
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace CCDModule 
{
	// =============================================================================
	// 1. CCD抽象基类定义
	// =============================================================================
	class ICCDDevice 
	{
	public:
		virtual ~ICCDDevice() = default;

		// 参数设置
		virtual bool   Connect()=0;
		virtual bool   DisConnect() = 0;
		virtual bool   SetExposureTime(double timeMs) = 0;
		virtual bool   GetExposureTime(double* timeMs) = 0;
		virtual bool   GetPixelNum(int* size) = 0;
		virtual bool   GetAcquiredData(unsigned short * buff, unsigned long size) = 0;

		// 虚函数 - 提供默认实现，子类可以重写
		virtual bool HasCoolingModule() const { return false; }
		virtual bool HasGainModule() const { return false; }
	};
	// 制冷功能接口
	class ICoolingModule 
	{
	public:
		virtual ~ICoolingModule() = default;
		virtual bool SetCoolingTemperature(double temp) = 0;
		virtual double GetCurrentTemperature() = 0;
		virtual bool EnableCooling(bool enable) = 0;
	};

	// 增益功能接口
	class IGainModule 
	{
	public:
		virtual ~IGainModule() = default;
		virtual bool SetGain(int gain) = 0;
		virtual int GetGain() = 0;
	};
	// =============================================================================
	// 2. 设备工厂接口
	// =============================================================================
	class ICCDFactory {
	public:
		virtual ~ICCDFactory() = default;

		// 获取设备类型（如 "TypeA", "TypeB"）
		virtual std::string GetDeviceType() const = 0;
		// 枚举可用设备
		virtual std::vector<std::string> EnumerateDevices() = 0;
		// 创建设备实例
		virtual std::unique_ptr<ICCDDevice> CreateDevice(const std::string& deviceId) = 0;
		// 检查设备是否支持
		virtual bool IsDeviceSupported(const std::string& deviceId) = 0;
	};

	// =============================================================================
	// 3. 动态库接口定义
	// =============================================================================
	extern "C" {
		// 每个CCD动态库必须实现的导出函数
		using CreateFactoryFunc = ICCDFactory* (*)();
		using DestroyFactoryFunc = void(*)(ICCDFactory*);
		using GetPluginInfoFunc = const char* (*)();
		using GetPluginVersionFunc = int(*)();
	}

#ifdef _WIN32
#define PLUGIN_EXPORT __declspec(dllexport)
#else
#define PLUGIN_EXPORT __attribute__((visibility("default")))
#endif

#define PLUGIN_EXPORT_FUNCTIONS() \
    extern "C" PLUGIN_EXPORT ICCDFactory* CreateFactory(); \
    extern "C" PLUGIN_EXPORT void DestroyFactory(ICCDFactory* factory); \
    extern "C" PLUGIN_EXPORT const char* GetPluginInfo(); \
    extern "C" PLUGIN_EXPORT int GetPluginVersion();

	// =============================================================================
	// 4. CCD管理器主类
	// =============================================================================
	class CCDManager {
	private:
		struct PluginInfo {
			HMODULE handle; // 跨平台句柄（HMODULE 或 void*）
			std::unique_ptr<ICCDFactory> factory;
			std::string pluginPath;
			std::string deviceType;
			int version;
		};

		std::map<std::string, PluginInfo> m_plugins;
		std::map<std::string, std::unique_ptr<ICCDDevice>> m_devices;

	public:
		CCDManager() = default;
		~CCDManager();

		// 插件管理
		bool LoadPlugin(const std::string& dllPath);
		bool UnloadPlugin(const std::string& deviceType);
		void UnloadAllPlugins();
		std::vector<std::string> GetLoadedPluginTypes() const;
		// 设备管理
		std::vector<std::string> EnumerateAllDevices();
		std::vector<std::string> EnumerateDevicesByType(const std::string& deviceType);

		bool OpenDevice(const std::string& deviceId, const std::string& alias = "");
		bool CloseDevice(const std::string& deviceId);
		void CloseAllDevices();

		ICCDDevice* GetDevice(const std::string& deviceId);
		std::vector<std::string> GetOpenDevices() const;

		// 批量操作
		bool SetExposureTimeAll(double timeMs);

	private:
		std::string GetDeviceTypeFromId(const std::string& deviceId);
		ICCDFactory* GetFactoryByType(const std::string& deviceType);
	};

} // namespace CCDModule
