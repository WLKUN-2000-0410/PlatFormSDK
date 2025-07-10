
#include "CDConfigManager.h"
#include <fstream>
#include "GlobalShare.h"
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
	std::ofstream file(filePath);
	if (!file.is_open()) {
		LogPrintErr("Failed to open config file for writing: " + filePath);
		return false;
	}
	file << GenerateConfigContent();
	file.close();
	return true;
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
	m_deviceConfigs[type] = config;
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

bool CCDConfigManager::EnsureConfigFile()
{
	std::vector<std::string> configPaths =
	{
		"./Ccd_config.json",
		//"./config/config.ini",
		//"C:/CCDModule/config.ini"
	};

	auto& configMgr = CCDConfigManager::GetInstance();

	// 尝试加载现有配置文件
	for (const auto& path : configPaths)
	{
		if (configMgr.LoadConfig(path))
		{
			LogPrintInfo("Config loaded from: {0}", path);
			return true;
		}
	}

	// 如果没有找到配置文件，创建默认配置
	LogPrintInfo("No config file found, creating default config");

	// 设置全局默认参数
	configMgr.SetGlobalParameter("DefaultCCDType", "SGM30");

	// 设置各设备默认配置
	CCDConfig  sgm30Config;
	sgm30Config.type = CCDType::SGM30;
	sgm30Config.Dependentfiles.push_back("UserApplication.dll");
	sgm30Config.Dependentfiles.push_back("SiUSBXp.dll");
	sgm30Config.Dependentfiles.push_back("DeviceLibrary.dll");
	sgm30Config.defaultExposureTime = 100.0;
	sgm30Config.defaulttemperature = 20.5;
	sgm30Config.defaultgain = 1;
	configMgr.SetDeviceConfig(CCDType::SGM30, sgm30Config);

	//CCDConfig c2Config(CCDType::C2);
	//c2Config.deviceName = "C2_Device";
	//c2Config.port = "COM4";
	//c2Config.baudRate = 9600;
	//c2Config.defaultExposureTime = 50.0;
	//c2Config.pixelNum = 1024;
	//configMgr.SetDeviceConfig(CCDType::C2, c2Config);

	// 保存默认配置
	return configMgr.SaveConfig("./Ccd_config.json");
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
	json j;
	j["Global"] = json::object();
	for (std::map<std::string, std::string>::const_iterator it = m_globalParams.begin();
		it != m_globalParams.end(); ++it) {
		j["Global"][it->first] = it->second;
	}
	j["Devices"] = json::object();
	for (std::map<CCDType, CCDConfig>::const_iterator it = m_deviceConfigs.begin();
		it != m_deviceConfigs.end(); ++it) {
		json cfg;
		cfg["Dependentfiles"] = json::array();
		for (std::vector<std::string>::const_iterator file_it = it->second.Dependentfiles.begin();
			file_it != it->second.Dependentfiles.end(); ++file_it) {
			cfg["Dependentfiles"].push_back(*file_it);
		}
		cfg["DefaultExposureTime"] = it->second.defaultExposureTime;
		cfg["Defaulttemperature"] = it->second.defaulttemperature;
		cfg["Defaultgain"] = it->second.defaultgain;
		j["Devices"][CCDTypeToString(it->first)] = cfg;
	}
	return j.dump(2); // 格式化输出，缩进2空格
}

void CCDConfigManager::SetLastError(SDKErrorCode code, const char* format, ...) 
{
	GlobalShare::g_s_lastError = code;

	char errorMessage[512] = { 0 };
	if (format) {
		va_list args;
		va_start(args, format);
		vsnprintf(errorMessage, sizeof(errorMessage), format, args);
		va_end(args);
		strcpy_s(GlobalShare::g_s_lastErrorMessage, errorMessage);
	}
	else {
		strcpy_s(GlobalShare::g_s_lastErrorMessage, GetErrorMessage(code));
		strcpy_s(errorMessage, GlobalShare::g_s_lastErrorMessage);
	}

//	LogPrintErr("[SDK_ERROR] Code: {0}, Message: {1}", code, errorMessage);
}
