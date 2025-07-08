
#include "Cvirtualccd.h"


Cvirtualccd::Cvirtualccd()
{
}


Cvirtualccd::~Cvirtualccd()
{
}

bool Cvirtualccd::Initialize(const CCDConfig & config)
{
	return false;
}

bool Cvirtualccd::IsConnected()
{
	return false;
}

bool Cvirtualccd::Connect()
{
	return false;
}

bool Cvirtualccd::DisConnect()
{
	return false;
}

bool Cvirtualccd::SetExposureTime(double timeMs)
{
	return false;
}

bool Cvirtualccd::GetExposureTime(double * timeMs)
{
	return false;
}

bool Cvirtualccd::GetPixelNum(int * size)
{
	return false;
}
