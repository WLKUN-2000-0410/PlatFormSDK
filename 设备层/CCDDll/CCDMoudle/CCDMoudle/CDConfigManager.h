#pragma once
#include "CCDInterface.h"
#include "CCDTypes.h"
#include <string>
#include <map>

class CCDConfigManager
{
private:
	CCDConfigManager();
	~CCDConfigManager();
public:
	static CCDConfigManager& GetInstance();

	// ���ļ���������
	bool LoadConfig(const std::string& filePath);

	// �������õ��ļ�
	bool SaveConfig(const std::string& filePath);

	// ��ȡ�豸����
	CCDConfig GetDeviceConfig(CCDType type) const;

	// �����豸����
	void SetDeviceConfig(CCDType type, const CCDConfig& config);

	// ��ȡȫ�����ò���
	std::string GetGlobalParameter(const std::string& key) const;

	// ����ȫ�����ò���
	void SetGlobalParameter(const std::string& key, const std::string& value);


private:
	std::map<CCDType, CCDConfig> m_deviceConfigs;
	std::map<std::string, std::string> m_globalParams;

	// ���������ļ�
	bool ParseConfigFile(const std::string& content);

	// ���������ļ�����
	std::string GenerateConfigContent() const;
};

