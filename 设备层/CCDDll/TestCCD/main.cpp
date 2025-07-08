#include <iostream>
#include "../CCDMoudle/CCDMoudle/CCDMoudle.h"

int main()
{
	if (InitDll())
	{
		Connect();
	}

	return 0;
}