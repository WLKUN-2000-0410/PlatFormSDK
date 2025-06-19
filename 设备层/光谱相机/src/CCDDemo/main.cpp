#include <iostream>
#include "CCDModule.h"

using namespace CCDModule;

int main()
{
	CCDManager ccdManager;

	// ���ظ���CCD���
	ccdManager.LoadPlugin("CCDTypeSgm30.dll");
	ccdManager.LoadPlugin("CCDTypeDFiled.dll");
	//ccdManager.LoadPlugin("CCDTypeC.dll");

	// ö�������豸
	auto allDevices = ccdManager.EnumerateAllDevices();
	for (const auto& device : allDevices) {
		std::cout << "Found device: " << device << std::endl;
	}

	// ���豸
	if (ccdManager.OpenDevice("TypeA:Device001", "CCD1")) {
		ICCDDevice* ccd1 = ccdManager.GetDevice("CCD1");
		if (ccd1) {
			ccd1->SetExposureTime(100.0);
			ccd1->SetGain(1.5);
		}

	}

	// ��������
	ccdManager.SetExposureTimeAll(200.0);

	// ����
	ccdManager.CloseAllDevices();
	ccdManager.UnloadAllPlugins();

	return 0;
}