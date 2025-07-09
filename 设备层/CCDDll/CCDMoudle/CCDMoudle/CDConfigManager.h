#pragma once
#include "CCDInterface.h"
#include "CCDTypes.h"
#include <string>
#include <map>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#define LogPrintInfo CCDConfigManager::GetInstance().getLogHandle()->info
#define LogPrintErr  CCDConfigManager::GetInstance().getLogHandle()->error


class CCDConfigManager
{
private:
	CCDConfigManager();
public:
	~CCDConfigManager();
public:
	static CCDConfigManager& GetInstance()
	{
		static CCDConfigManager instance;
		return instance;
	}

	// 从文件加载配置
	bool LoadConfig(const std::string& filePath);

	// 保存配置到文件
	bool SaveConfig(const std::string& filePath);

	// 获取设备配置
	CCDConfig GetDeviceConfig(CCDType type) const;

	// 设置设备配置
	void SetDeviceConfig(CCDType type, const CCDConfig& config);

	// 获取全局配置参数
	std::string GetGlobalParameter(const std::string& key) const;

	// 设置全局配置参数
	void SetGlobalParameter(const std::string& key, const std::string& value);
	//
	void initLog();
	//
	std::shared_ptr<spdlog::logger>  getLogHandle();

private:
	std::map<CCDType, CCDConfig> m_deviceConfigs;
	std::map<std::string, std::string> m_globalParams;

	// 解析配置文件
	bool ParseConfigFile(const std::string& content);

	// 生成配置文件内容
	std::string GenerateConfigContent() const;
	//
	std::shared_ptr<spdlog::logger> logger;
};

