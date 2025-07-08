
#include "CCDManager.h"
#include "CCDFactory.h"

CCDManager & CCDManager::GetInstance()
{
	static CCDManager instance;
	return instance;
}

int CCDManager::CreateDevice(CCDType type)
{
	auto device = CCDFactory::CreateDevice(type);
	if (device) {
		int deviceId = m_nextDeviceId++;
		m_devices[deviceId] = device;
		return deviceId;
	}
	return -1; // 创建失败
}

std::shared_ptr<CCDBase> CCDManager::GetDevice(int deviceId)
{
	auto it = m_devices.find(deviceId);
	if (it != m_devices.end()) {
		return it->second;
	}
	return nullptr;
}

bool CCDManager::DestroyDevice(int deviceId)
{
	auto it = m_devices.find(deviceId);
	if (it != m_devices.end()) {
		// 确保设备断开连接
		it->second->DisConnect();
		m_devices.erase(it);
		return true;
	}
	return false;
}

std::vector<int> CCDManager::GetAllDeviceIds() const
{
	std::vector<int> ids;
	for (const auto& pair : m_devices) {
		ids.push_back(pair.first);
	}
	return ids;
}

void CCDManager::ClearAllDevices()
{
	for (auto& pair : m_devices) {
		pair.second->DisConnect();
	}
	m_devices.clear();
}
