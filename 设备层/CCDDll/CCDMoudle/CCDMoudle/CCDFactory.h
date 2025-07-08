#pragma once
#include "CCDInterface.h"
#include "CCDTypes.h"
#include <memory>
#include <vector>

class CCDFactory {
public:
	// 创建指定类型的CCD设备
	static std::shared_ptr<CCDBase> CreateDevice(CCDType type);

	// 检查设备类型是否支持
	static bool IsTypeSupported(CCDType type);

	// 获取所有支持的设备类型
	static std::vector<CCDType> GetSupportedTypes();
};

