#pragma once
#pragma once

#include <string>
#include <vector>
#include <map>

// CCD�豸����ö��
enum  CCDType {
	A1,
	A2,
	A3,
	A4,
	D1,
	D3,
	C2,
	SGM30,
	VIRTUAL
};

// CCD���ýṹ
struct CCDConfig 
{
	CCDType type;
	std::string deviceName;
	std::string port;
	int baudRate;
	double defaultExposureTime;
	int pixelNum;
	std::map<std::string, std::string> parameters;

	// Ĭ�Ϲ��캯��
	CCDConfig() : type(CCDType::VIRTUAL), baudRate(9600), defaultExposureTime(100.0), pixelNum(1024) {}

	// �����͵Ĺ��캯��
	CCDConfig(CCDType t) : type(t), baudRate(9600), defaultExposureTime(100.0), pixelNum(1024) {}
};

// CCD�������
struct CCDResult {
	bool success;
	std::string message;
	std::vector<uint8_t> data;

	CCDResult(bool s = false, const std::string& msg = "") : success(s), message(msg) {}
};

// ���ߺ������ַ���תCCD����
CCDType StringToCCDType(const std::string& typeStr);

// ���ߺ�����CCD����ת�ַ���
std::string CCDTypeToString(CCDType type);
