// =============================================================================
// �ļ�: include/CCDModule.h
// ����: CCDģ��ͳһ�ܹ��Ĺ����ӿڶ��壬�����豸�ӿڡ������ӿںͲ��������
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
	// 1. CCD������ඨ��
	// =============================================================================
	class ICCDDevice 
	{
	public:
		virtual ~ICCDDevice() = default;

		// ��������
		virtual bool   Connect()=0;
		virtual bool   DisConnect() = 0;
		virtual bool   SetExposureTime(double timeMs) = 0;
		virtual bool   GetExposureTime(double* timeMs) = 0;
		virtual bool   GetPixelNum(int* size) = 0;
		virtual bool   GetAcquiredData(unsigned short * buff, unsigned long size) = 0;

		// �麯�� - �ṩĬ��ʵ�֣����������д
		virtual bool HasCoolingModule() const { return false; }
		virtual bool HasGainModule() const { return false; }
	};
	// ���书�ܽӿ�
	class ICoolingModule 
	{
	public:
		virtual ~ICoolingModule() = default;
		virtual bool SetCoolingTemperature(double temp) = 0;
		virtual double GetCurrentTemperature() = 0;
		virtual bool EnableCooling(bool enable) = 0;
	};

	// ���湦�ܽӿ�
	class IGainModule 
	{
	public:
		virtual ~IGainModule() = default;
		virtual bool SetGain(int gain) = 0;
		virtual int GetGain() = 0;
	};
	// =============================================================================
	// 2. �豸�����ӿ�
	// =============================================================================
	class ICCDFactory {
	public:
		virtual ~ICCDFactory() = default;

		// ��ȡ�豸���ͣ��� "TypeA", "TypeB"��
		virtual std::string GetDeviceType() const = 0;
		// ö�ٿ����豸
		virtual std::vector<std::string> EnumerateDevices() = 0;
		// �����豸ʵ��
		virtual std::unique_ptr<ICCDDevice> CreateDevice(const std::string& deviceId) = 0;
		// ����豸�Ƿ�֧��
		virtual bool IsDeviceSupported(const std::string& deviceId) = 0;
	};

	// =============================================================================
	// 3. ��̬��ӿڶ���
	// =============================================================================
	extern "C" {
		// ÿ��CCD��̬�����ʵ�ֵĵ�������
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
	// 4. CCD����������
	// =============================================================================
	class CCDManager {
	private:
		struct PluginInfo {
			HMODULE handle; // ��ƽ̨�����HMODULE �� void*��
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

		// �������
		bool LoadPlugin(const std::string& dllPath);
		bool UnloadPlugin(const std::string& deviceType);
		void UnloadAllPlugins();
		std::vector<std::string> GetLoadedPluginTypes() const;
		// �豸����
		std::vector<std::string> EnumerateAllDevices();
		std::vector<std::string> EnumerateDevicesByType(const std::string& deviceType);

		bool OpenDevice(const std::string& deviceId, const std::string& alias = "");
		bool CloseDevice(const std::string& deviceId);
		void CloseAllDevices();

		ICCDDevice* GetDevice(const std::string& deviceId);
		std::vector<std::string> GetOpenDevices() const;

		// ��������
		bool SetExposureTimeAll(double timeMs);

	private:
		std::string GetDeviceTypeFromId(const std::string& deviceId);
		ICCDFactory* GetFactoryByType(const std::string& deviceType);
	};

} // namespace CCDModule
