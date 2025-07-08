#pragma once
#include "CCDInterface.h"
#include <memory>
#include <vector>
#include <map>
#include "CCDTypes.h"

class CCDManager 
{
private:
	CCDManager() = default;
	~CCDManager() = default;
public:
	static CCDManager& GetInstance();

	// 创建CCD设备
	int CreateDevice(CCDType type);

	// 获取设备实例
	std::shared_ptr<CCDBase> GetDevice(int deviceId);

	// 销毁设备
	bool DestroyDevice(int deviceId);

	// 获取所有设备ID
	std::vector<int> GetAllDeviceIds() const;

	// 清理所有设备
	void ClearAllDevices();


private:
	std::map<int, std::shared_ptr<CCDBase>> m_devices;
	int m_nextDeviceId = 1;
};
