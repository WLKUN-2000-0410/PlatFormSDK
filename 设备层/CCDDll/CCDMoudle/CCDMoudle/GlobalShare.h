#pragma once
#include <memory>
#include "CCDInterface.h"
#include <mutex>
#include <string>
#include "CCDMoudle.h"

class GlobalShare
{
public:
	static   std::shared_ptr<CCDBase>		g_currentDevice;
	static   CCDType					    g_currentType;
	static   bool							g_initialized;
	static   std::mutex                     g_mutex;               // �̰߳�ȫ����
	static   std::string                    g_lastError;           // ������Ϣ��¼
	static   SDKErrorCode                   g_s_lastError;
	static   char							g_s_lastErrorMessage[512];
};

