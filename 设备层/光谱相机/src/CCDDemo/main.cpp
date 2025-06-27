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
	if (ccdManager.OpenDevice("sgm30:Device001", "CCD1")) {
		ICCDDevice* ccd1 = ccdManager.GetDevice("CCD1");
		if (ccd1) {
			ccd1->Connect();
			if (ccd1->HasCoolingModule())
			{
				IGainModule *GainDevice = dynamic_cast<IGainModule*>(ccd1);
				GainDevice->SetGain(1);
			}
			if (ccd1->HasCoolingModule())
			{
				ICoolingModule *CoolDevice = dynamic_cast<ICoolingModule*>(ccd1);
				CoolDevice->SetCoolingTemperature(-10.0);
			}
			int size = 0;
			if (ccd1->GetPixelNum(&size))
			{
				std::vector<unsigned short> vbuff(size, 0);
				 
				ccd1->GetAcquiredData(vbuff.data(), size);

				int a = 9;
			}
		}

	}
	//if (ccdManager.OpenDevice("DFiled:Device001", "CCD1")) {
	//	ICCDDevice* ccd1 = ccdManager.GetDevice("CCD1");
	//	if (ccd1) {
	//		ccd1->Connect();
	//		if (ccd1->HasCoolingModule())
	//		{
	//			IGainModule *GainDevice = dynamic_cast<IGainModule*>(ccd1);
	//			GainDevice->SetGain(1);
	//		}
	//		if (ccd1->HasCoolingModule())
	//		{
	//			ICoolingModule *CoolDevice = dynamic_cast<ICoolingModule*>(ccd1);
	//			CoolDevice->SetCoolingTemperature(-10.0);
	//		}
	//	}

	//}

	// ��������
	ccdManager.SetExposureTimeAll(200.0);

	// ����
	ccdManager.CloseAllDevices();
	ccdManager.UnloadAllPlugins();

	system("pause");
	return 0;
}