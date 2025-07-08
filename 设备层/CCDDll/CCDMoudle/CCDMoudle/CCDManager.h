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

	// ����CCD�豸
	int CreateDevice(CCDType type);

	// ��ȡ�豸ʵ��
	std::shared_ptr<CCDBase> GetDevice(int deviceId);

	// �����豸
	bool DestroyDevice(int deviceId);

	// ��ȡ�����豸ID
	std::vector<int> GetAllDeviceIds() const;

	// ���������豸
	void ClearAllDevices();


private:
	std::map<int, std::shared_ptr<CCDBase>> m_devices;
	int m_nextDeviceId = 1;
};
