
#include "CDConfigManager.h"
#include <fstream>
#include <sstream>

CCDConfigManager::CCDConfigManager()
{
}


CCDConfigManager::~CCDConfigManager()
{
}

CCDConfigManager & CCDConfigManager::GetInstance()
{
	static CCDConfigManager instance;
	return instance;
}

bool CCDConfigManager::LoadConfig(const std::string & filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open()) {
		return false;
	}

	std::string content((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
	file.close();

	return ParseConfigFile(content);
}

bool CCDConfigManager::SaveConfig(const std::string & filePath)
{
	return false;
}

CCDConfig CCDConfigManager::GetDeviceConfig(CCDType type) const
{
	auto it = m_deviceConfigs.find(type);
	if (it != m_deviceConfigs.end()) {
		return it->second;
	}

	// 返回默认配置
	CCDConfig defaultConfig;
	defaultConfig.type = type;
	return defaultConfig;
}

void CCDConfigManager::SetDeviceConfig(CCDType type, const CCDConfig & config)
{
}

std::string CCDConfigManager::GetGlobalParameter(const std::string & key) const
{
	auto it = m_globalParams.find(key);
	if (it != m_globalParams.end()) {
		return it->second;
	}
	return "";
}

void CCDConfigManager::SetGlobalParameter(const std::string & key, const std::string & value)
{
	m_globalParams[key] = value;
}

bool CCDConfigManager::ParseConfigFile(const std::string & content)
{
	// 简单的配置解析示例（实际项目中建议使用JSON或XML库）
	std::istringstream iss(content);
	std::string line;

	while (std::getline(iss, line)) {
		if (line.empty() || line[0] == '#') continue;

		size_t pos = line.find('=');
		if (pos != std::string::npos) {
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 1);
			SetGlobalParameter(key, value);
		}
	}

	return true;
}

std::string CCDConfigManager::GenerateConfigContent() const
{
	return std::string();
}
