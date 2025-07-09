
#include "CDConfigManager.h"
#include <fstream>
#include <sstream>

using json = nlohmann::json;

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
	logger->set_level(spdlog::level::info);
	logger->flush_on(spdlog::level::info); // 设置在info级别以上的日志才会刷新到文件
	spdlog::set_default_logger(logger); // 设置默认日志对象

}

std::shared_ptr<spdlog::logger> CCDConfigManager::getLogHandle()
{
	return logger;
}

bool CCDConfigManager::ParseConfigFile(const std::string & content)
{
	try {
		json j = json::parse(content);
		// 解析全局参数
		if (j.find("Global") != j.end() && j["Global"].is_object()) {
			for (json::iterator it = j["Global"].begin(); it != j["Global"].end(); ++it) {
				std::string key = it.key();
				json value = it.value();
				m_globalParams[key] = value.is_string() ? value.get<std::string>() : value.dump();
			}
		}
		else
		{
			LogPrintErr("json file : 'Global' was not found");
			return false;
		}
		// 解析设备配置
		if (j.find("Devices") != j.end() && j["Devices"].is_object()) 
		{
			for (json::iterator it = j["Devices"].begin(); it != j["Devices"].end(); ++it) 
			{
				std::string typeStr = it.key();
				json config = it.value();
				CCDType type = StringToCCDType(typeStr);
				CCDConfig cfg;
				cfg.type = type;
				if (config.find("Dependentfiles") != config.end() && config["Dependentfiles"].is_array()) 
				{
					cfg.Dependentfiles.clear();
					for (json::const_iterator file_it = config["Dependentfiles"].begin();
						file_it != config["Dependentfiles"].end(); ++file_it) {
						cfg.Dependentfiles.push_back(file_it->get<std::string>());
					}
				}
				else
				{
					LogPrintErr("json {0} file : 'Dependentfiles' was not found", typeStr);
					return false;
				}
				// 解析 DefaultExposureTime
				if (config.find("DefaultExposureTime") != config.end() && config["DefaultExposureTime"].is_number()) 
				{
					cfg.defaultExposureTime = config["DefaultExposureTime"].get<double>();
				}
				else 
				{
					LogPrintErr("json {0} file : 'DefaultExposureTime' was not found", typeStr);
					return false;
				}

				// 解析 Defaulttemperature
				if (config.find("Defaulttemperature") != config.end() && config["Defaulttemperature"].is_number()) 
				{
					cfg.defaulttemperature = config["Defaulttemperature"].get<double>();
				}
				else 
				{
					LogPrintErr("json {0} file : 'Defaulttemperature' was not found", typeStr);
					return false;
				}

				// 解析 Defaultgain
				if (config.find("Defaultgain") != config.end() && config["Defaultgain"].is_number_integer()) 
				{
					cfg.defaultgain = config["Defaultgain"].get<int>();
				}
				else 
				{
					LogPrintErr("json {0} file : 'Defaultgain' was not found", typeStr);
					return false;
				}
				m_deviceConfigs[type] = cfg;
			}
		}
		else
		{
			LogPrintErr("json file : 'Devices' was not found");
			return false;
		}
		return true;
	}
	catch (const json::exception& e) 
	{
		LogPrintErr("Failed to read the json file!");
		return false;
	}
}

std::string CCDConfigManager::GenerateConfigContent() const
{
	return std::string();
}
