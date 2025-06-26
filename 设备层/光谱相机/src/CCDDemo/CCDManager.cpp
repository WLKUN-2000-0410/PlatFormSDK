#include "CCDModule.h"


namespace CCDModule
{
	CCDManager::~CCDManager() {
		CloseAllDevices();
		UnloadAllPlugins();
	}

	bool CCDManager::LoadPlugin(const std::string& dllPath) {
		HMODULE handle = LoadLibraryA(dllPath.c_str());
		if (!handle) {
			return false;
		}

		// 获取导出函数
		auto createFactory = (CreateFactoryFunc)GetProcAddress(handle, "CreateFactory");
		auto destroyFactory = (DestroyFactoryFunc)GetProcAddress(handle, "DestroyFactory");
		auto getPluginInfo = (GetPluginInfoFunc)GetProcAddress(handle, "GetPluginInfo");
		auto getPluginVersion = (GetPluginVersionFunc)GetProcAddress(handle, "GetPluginVersion");

		if (!createFactory || !destroyFactory || !getPluginInfo || !getPluginVersion) {
			FreeLibrary(handle);
			return false;
		}

		// 创建工厂实例
		auto factory = std::unique_ptr<ICCDFactory>(createFactory());
		if (!factory) {
			FreeLibrary(handle);
			return false;
		}

		std::string deviceType = factory->GetDeviceType();

		// 检查是否已加载同类型插件
		if (m_plugins.find(deviceType) != m_plugins.end()) {
			return false; // 已存在同类型插件
		}

		// 保存插件信息
		PluginInfo info;
		info.handle = handle;
		info.factory = std::move(factory);
		info.pluginPath = dllPath;
		info.deviceType = deviceType;
		info.version = getPluginVersion();

		m_plugins[deviceType] = std::move(info);
		return true;
	}

	bool CCDManager::UnloadPlugin(const std::string& deviceType) {
		auto it = m_plugins.find(deviceType);
		if (it == m_plugins.end()) {
			return false;
		}

		// 关闭该类型的所有设备
		std::vector<std::string> devicesToClose;
		for (const auto& device : m_devices) {
			if (GetDeviceTypeFromId(device.first) == deviceType) {
				devicesToClose.push_back(device.first);
			}
		}

		for (const auto& deviceId : devicesToClose) {
			CloseDevice(deviceId);
		}

		// 卸载插件
		if (it->second.handle) {
			FreeLibrary(it->second.handle);
		}

		m_plugins.erase(it);
		return true;
	}

	void CCDManager::UnloadAllPlugins() {
		CloseAllDevices();

		for (auto& plugin : m_plugins) 
		{
			plugin.second.factory.reset();
			if (plugin.second.handle) {
				FreeLibrary(plugin.second.handle);
			}
		}
		m_plugins.clear();
	}

	std::vector<std::string> CCDManager::GetLoadedPluginTypes() const {
		std::vector<std::string> types;
		for (const auto& plugin : m_plugins) {
			types.push_back(plugin.first);
		}
		return types;
	}

	std::vector<std::string> CCDManager::EnumerateAllDevices() {
		std::vector<std::string> allDevices;

		for (auto& plugin : m_plugins) {
			auto devices = plugin.second.factory->EnumerateDevices();
			for (const auto& device : devices) {
				allDevices.push_back(plugin.first + ":" + device);
			}
		}

		return allDevices;
	}

	std::vector<std::string> CCDManager::EnumerateDevicesByType(const std::string& deviceType) {
		auto it = m_plugins.find(deviceType);
		if (it == m_plugins.end()) {
			return{};
		}

		return it->second.factory->EnumerateDevices();
	}

	bool CCDManager::OpenDevice(const std::string& deviceId, const std::string& alias) {
		std::string deviceType = GetDeviceTypeFromId(deviceId);
		ICCDFactory* factory = GetFactoryByType(deviceType);

		if (!factory) {
			return false;
		}

		// 提取实际设备ID（去掉类型前缀）
		size_t colonPos = deviceId.find(':');
		std::string actualDeviceId = (colonPos != std::string::npos) ?
			deviceId.substr(colonPos + 1) : deviceId;

		auto device = factory->CreateDevice(actualDeviceId);
		//if (!device || !device->Initialize()) {
		//	return false;
		//}

		std::string key = alias.empty() ? deviceId : alias;
		m_devices[key] = std::move(device);
		return true;
	}

	bool CCDManager::CloseDevice(const std::string& deviceId) {
		auto it = m_devices.find(deviceId);
		if (it == m_devices.end()) {
			return false;
		}

//		it->second->Finalize();
		m_devices.erase(it);
		return true;
	}

	void CCDManager::CloseAllDevices() {
		for (auto& device : m_devices) {
			device.second->DisConnect();
		}
		m_devices.clear();
	}

	ICCDDevice* CCDManager::GetDevice(const std::string& deviceId) {
		auto it = m_devices.find(deviceId);
		return (it != m_devices.end()) ? it->second.get() : nullptr;
	}

	std::vector<std::string> CCDManager::GetOpenDevices() const {
		std::vector<std::string> devices;
		for (const auto& device : m_devices) {
			devices.push_back(device.first);
		}
		return devices;
	}

	bool CCDManager::SetExposureTimeAll(double timeMs) {
		bool allSuccess = true;
		for (auto& device : m_devices) {
			if (!device.second->SetExposureTime(timeMs)) {
				allSuccess = false;
			}
		}
		return allSuccess;
	}

	std::string CCDManager::GetDeviceTypeFromId(const std::string& deviceId) {
		size_t colonPos = deviceId.find(':');
		if (colonPos != std::string::npos) {
			return deviceId.substr(0, colonPos);
		}

		// 如果没有类型前缀，尝试查找支持该设备的插件
		for (auto& plugin : m_plugins) {
			if (plugin.second.factory->IsDeviceSupported(deviceId)) {
				return plugin.first;
			}
		}

		return "";
	}

	ICCDFactory* CCDManager::GetFactoryByType(const std::string& deviceType) {
		auto it = m_plugins.find(deviceType);
		return (it != m_plugins.end()) ? it->second.factory.get() : nullptr;
	}

}