
#include "CCDFactory.h"
#include "CCDTypeSgm30.h"
#include "CCDTypeC2.h"



std::shared_ptr<CCDBase> CCDFactory::CreateDevice(CCDType type)
{
	switch (type)
	{
	case CCDType::SGM30:
		return std::make_shared<CCDTypeSgm30>();
	case CCDType::C2:
		return std::make_shared<CCDTypeC2>();
	default:
		return nullptr;
	}
}

bool CCDFactory::IsTypeSupported(CCDType type)
{
	return CreateDevice(type) != nullptr;
}

std::vector<CCDType> CCDFactory::GetSupportedTypes()
{
	return{
		CCDType::A1, CCDType::A2, CCDType::A3, CCDType::A4,
		CCDType::D1, CCDType::D3, CCDType::C2, CCDType::SGM30,
		CCDType::VIRTUAL
	};
}
