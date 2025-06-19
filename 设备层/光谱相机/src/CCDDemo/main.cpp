#include <iostream>
#include "CCDModule.h"

using namespace CCDModule;

int main()
{
	CCDManager ccdManager;

	// 加载各种CCD插件
	ccdManager.LoadPlugin("CCDTypeSgm30.dll");
	ccdManager.LoadPlugin("CCDTypeDFiled.dll");
	//ccdManager.LoadPlugin("CCDTypeC.dll");

	// 枚举所有设备
	auto allDevices = ccdManager.EnumerateAllDevices();
	for (const auto& device : allDevices) {
		std::cout << "Found device: " << device << std::endl;
	}

	// 打开设备
	if (ccdManager.OpenDevice("TypeA:Device001", "CCD1")) {
		ICCDDevice* ccd1 = ccdManager.GetDevice("CCD1");
		if (ccd1) {
			ccd1->SetExposureTime(100.0);
			ccd1->SetGain(1.5);
		}

	}

	// 批量操作
	ccdManager.SetExposureTimeAll(200.0);

	// 清理
	ccdManager.CloseAllDevices();
	ccdManager.UnloadAllPlugins();

	return 0;
}