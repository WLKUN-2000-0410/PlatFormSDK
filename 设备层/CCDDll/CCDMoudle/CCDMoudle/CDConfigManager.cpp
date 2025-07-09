
#include "CDConfigManager.h"
#include <fstream>
#include <sstream>

CCDConfigManager::CCDConfigManager()
{
	initLog();
}


CCDConfigManager::~CCDConfigManager()
{
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

void CCDConfigManager::initLog()
{
	logger = spdlog::basic_logger_mt("realtime_logger", "log.txt");
	logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%L%$] [thread %t] %v"); // 设置日志格式
	logger->flush_on(spdlog::level::info); // 设置在info级别以上的日志才会刷新到文件
	spdlog::set_default_logger(logger); // 设置默认日志对象

}

std::shared_ptr<spdlog::logger> CCDConfigManager::getLogHandle()
{
	return logger;
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
