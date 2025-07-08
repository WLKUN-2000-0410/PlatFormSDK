#include "CCDTypes.h"


CCDType StringToCCDType(const std::string& typeStr) {
	if (typeStr == "A1") return CCDType::A1;
	if (typeStr == "A2") return CCDType::A2;
	if (typeStr == "A3") return CCDType::A3;
	if (typeStr == "A4") return CCDType::A4;
	if (typeStr == "D1") return CCDType::D1;
	if (typeStr == "D3") return CCDType::D3;
	if (typeStr == "C2") return CCDType::C2;
	if (typeStr == "SGM30") return CCDType::SGM30;
	if (typeStr == "VIRTUAL") return CCDType::VIRTUAL;
	return CCDType::VIRTUAL; // Ä¬ÈÏÖµ
}

std::string CCDTypeToString(CCDType type) {
	switch (type) {
	case CCDType::A1: return "A1";
	case CCDType::A2: return "A2";
	case CCDType::A3: return "A3";
	case CCDType::A4: return "A4";
	case CCDType::D1: return "D1";
	case CCDType::D3: return "D3";
	case CCDType::C2: return "C2";
	case CCDType::SGM30: return "SGM30";
	case CCDType::VIRTUAL: return "VIRTUAL";
	default: return "UNKNOWN";
	}
}