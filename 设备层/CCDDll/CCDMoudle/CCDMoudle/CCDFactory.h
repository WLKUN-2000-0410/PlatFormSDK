#pragma once
#include "CCDInterface.h"
#include "CCDTypes.h"
#include <memory>
#include <vector>

class CCDFactory {
public:
	// ����ָ�����͵�CCD�豸
	static std::shared_ptr<CCDBase> CreateDevice(CCDType type);

	// ����豸�����Ƿ�֧��
	static bool IsTypeSupported(CCDType type);

	// ��ȡ����֧�ֵ��豸����
	static std::vector<CCDType> GetSupportedTypes();
};

