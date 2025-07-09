#pragma once
#include <memory>
#include "CCDInterface.h"
#include <mutex>
#include <string>

class GlobalShare
{
public:
	static   std::shared_ptr<CCDBase>		g_currentDevice;
	static   CCDType					    g_currentType;
	static   bool							g_initialized;
	static   std::mutex                     g_mutex;               // �̰߳�ȫ����
	static   std::string                    g_lastError;           // ������Ϣ��¼

};

