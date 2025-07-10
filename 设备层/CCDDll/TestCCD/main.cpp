#include <iostream>
#include "../CCDMoudle/CCDMoudle/CCDMoudle.h"
#include <vector>

int main()
{
	if (SDKInit())
	{
		char version[32] = "";
		GetSDKVersion(version);
		if (!Connect())
		{
			const char* ErrMsg = GetLastErrorMessage();
			printf("%s \r\n",ErrMsg);
		}
		else
		{
			int size = 0;
			if (GetPixelNum(&size))
			{
				std::vector<unsigned short> data(size, 0);
				if (DataAcqOneShot(data.data(), size))
				{
					printf("Successfully completed single-shot data acquisition.\r\n");
				}
			}

			DisConnect();
		}
		
	}
	SDKCleanup();
	return 0;
}