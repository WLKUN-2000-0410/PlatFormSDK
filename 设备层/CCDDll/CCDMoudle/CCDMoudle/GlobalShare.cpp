
#include "GlobalShare.h"

std::shared_ptr<CCDBase>    GlobalShare::g_currentDevice = nullptr;
CCDType					    GlobalShare::g_currentType   = CCDType::VIRTUAL;
bool						GlobalShare::g_initialized   = false;
std::mutex					GlobalShare::g_mutex;
std::string				    GlobalShare::g_lastError     = "";
SDKErrorCode                GlobalShare::g_s_lastError   = SDK_SUCCESS;
char					    GlobalShare::g_s_lastErrorMessage[512] ="";