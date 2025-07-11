#pragma once
#pragma once

#include <string>
#include <vector>
#include <map>

// CCD设备类型枚举
enum  CCDType {
	A1,
	A2,
	A3,
	A4,
	D1,
	D3,
	C2,
	SGM30,
	VIRTUAL
};

// CCD配置结构
struct CCDConfig 
{
	CCDType type;
	std::vector<std::string>  Dependentfiles;
	double defaultExposureTime;
	double defaulttemperature;
	int    defaultgain;
	std::vector<int>  GainRange;
	CCDConfig()
	{
		type = CCDType::VIRTUAL;
		Dependentfiles.swap(std::vector<std::string>());
		defaultExposureTime = 100.0;
		defaulttemperature = 20.5;
		defaultgain = 1;
		GainRange.swap(std::vector<int>());
	}
};

// CCD操作结果
struct CCDResult {
	bool success;
	std::string message;
	std::vector<uint8_t> data;

	CCDResult(bool s = false, const std::string& msg = "") : success(s), message(msg) {}
};

// 工具函数：字符串转CCD类型
CCDType StringToCCDType(const std::string& typeStr);

// 工具函数：CCD类型转字符串
std::string CCDTypeToString(CCDType type);
