
#include "CCDTypeSgm30.h"


CCDTypeSgm30::CCDTypeSgm30()
{
}


CCDTypeSgm30::~CCDTypeSgm30()
{
}

bool CCDTypeSgm30::Initialize(const CCDConfig & config)
{

	return true;
}

bool CCDTypeSgm30::IsConnected()
{
	return false;
}

bool CCDTypeSgm30::Connect()
{
	return false;
}

bool CCDTypeSgm30::DisConnect()
{
	return false;
}

bool CCDTypeSgm30::SetExposureTime(double timeMs)
{
	return false;
}

bool CCDTypeSgm30::GetExposureTime(double * timeMs)
{
	return false;
}

bool CCDTypeSgm30::GetPixelNum(int * size)
{
	return false;
}
