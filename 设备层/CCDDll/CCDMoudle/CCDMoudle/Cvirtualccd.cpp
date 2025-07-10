#include "Cvirtualccd.h"
#include "CDConfigManager.h"
#include <algorithm>

Cvirtualccd::Cvirtualccd()
	: m_exposureTime(100.0), m_temperature(20.5), m_gain(1),
	m_isConnected(false), m_coolingEnabled(false), m_pixelNum(1024) {
}

Cvirtualccd::~Cvirtualccd() {
}

bool Cvirtualccd::Initialize(const CCDConfig& config) 
{
	for (const auto& file : config.Dependentfiles) 
	{
		if (!file.empty()) 
		{
			LogPrintErr("VIRTUAL device does not support DLL dependencies: {0}",file);
			return false;
		}
	}
	m_exposureTime = config.defaultExposureTime;
	m_temperature = config.defaulttemperature;
	m_gain = config.defaultgain;
	m_isConnected = false;
	m_coolingEnabled = false;
	m_pixelNum = 1024;
	LogPrintInfo("VIRTUAL device initialized with exposure time: {0} , temperature: {1} , gain: {2}",
		m_exposureTime, m_temperature, m_gain);

	return true;
}

bool Cvirtualccd::IsConnected() {
	return m_isConnected;
}

bool Cvirtualccd::Connect() {
	m_isConnected = true;
	LogPrintInfo("VIRTUAL device connected");
	return true;
}

bool Cvirtualccd::DisConnect() {
	m_isConnected = false;
	LogPrintInfo("VIRTUAL device disconnected");
	return true;
}

bool Cvirtualccd::SetExposureTime(double timeMs) {
	if (timeMs <= 0) {
		LogPrintErr("Invalid exposure time: {0}",timeMs);
		return false;
	}
	m_exposureTime = timeMs;
	LogPrintInfo("VIRTUAL device set exposure time to: {0} ms", timeMs);
	return true;
}

bool Cvirtualccd::GetExposureTime(double* timeMs) {
	if (!timeMs) {
		LogPrintErr("Null pointer for exposure time");
		return false;
	}
	*timeMs = m_exposureTime;
	return true;
}

bool Cvirtualccd::SetGain(int gain) {
	if (gain <= 0) {
		LogPrintErr("Invalid gain: {0}",gain);
		return false;
	}
	m_gain = gain;
	LogPrintInfo("VIRTUAL device set gain to: {0}" ,gain);
	return true;
}

bool Cvirtualccd::GetGain(int* gain) {
	if (!gain) {
		LogPrintErr("Null pointer for gain");
		return false;
	}
	*gain = m_gain;
	return true;
}

bool Cvirtualccd::EnableCooling(bool enable) {
	m_coolingEnabled = enable;
	LogPrintInfo("VIRTUAL device cooling {0}" ,(enable ? "enabled" : "disabled"));
	return true;
}

bool Cvirtualccd::SetCoolingTemperature(double temp) {
	m_temperature = temp;
	LogPrintInfo("VIRTUAL device set temperature to: " + std::to_string(temp) + " C");
	return true;
}

bool Cvirtualccd::GetCurrentTemperature(double* temp) {
	if (!temp) {
		LogPrintErr("Null pointer for temperature");
		return false;
	}
	*temp = m_temperature;
	return true;
}
/*
bool Cvirtualccd::DataAcqOneShot(double* pd, int nPixSize) {
	if (!pd || nPixSize <= 0 || nPixSize > m_pixelNum) {
		LogPrintErr("Invalid parameters for DataAcqOneShot: pd=" + std::to_string((uintptr_t)pd) +
			", nPixSize=" + std::to_string(nPixSize));
		return false;
	}
	for (int i = 0; i < nPixSize; ++i) {
		pd[i] = static_cast<double>(i % 256); // Simulated data
	}
	LogPrintInfo("VIRTUAL device acquired one-shot data, size: " + std::to_string(nPixSize));
	return true;
}

bool Cvirtualccd::DataAcqOneShotImg(double* pdImg, int* nPixSize) {
	if (!pdImg || !nPixSize || *nPixSize <= 0 || *nPixSize > m_pixelNum) {
		LogPrintErr("Invalid parameters for DataAcqOneShotImg: pdImg=" + std::to_string((uintptr_t)pdImg) +
			", nPixSize=" + std::to_string(*nPixSize));
		return false;
	}
	for (int i = 0; i < *nPixSize; ++i) {
		pdImg[i] = static_cast<double>(i % 256); // Simulated image data
	}
	LogPrintInfo("VIRTUAL device acquired one-shot image, size: " + std::to_string(*nPixSize));
	return true;
}
*/
bool Cvirtualccd::GetPixelNum(int* size) {
	if (!size) {
		LogPrintErr("Null pointer for pixel number");
		return false;
	}
	*size = m_pixelNum;
	return true;
}

bool Cvirtualccd::DataAcqOneShot(unsigned short * buff, unsigned long size)
{
	if (!buff) {
		return false;
	}

	if (size == 0) {
		return false;
	}

	// ����豸�Ƿ�����
	if (!IsConnected()) {
		return false;
	}

	try {
		// ģ�����ݲɼ���ʱ�������ع�ʱ�䣩
		if (m_exposureTime > 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(m_exposureTime)));
		}

		// ����sin(x)ǰ1/2���ڵ�ģ������
		const double PI = 3.14159265358979323846;
		const double halfPeriod = PI;  // �� ��Ӧsin(x)��ǰ1/2����

									   // ��������
		const unsigned short baseValue = 1000;     // ����ƫ��ֵ
		const unsigned short amplitude = 30000;    // ������ʺ�16λ���ݷ�Χ��
		const unsigned short noiseLevel = 100;    // ����ˮƽ

												  // ����Ӱ�����
		double gainFactor = static_cast<double>(m_gain) / 100.0;  // ���������׼Ϊ100
		unsigned short actualAmplitude = static_cast<unsigned short>(amplitude * gainFactor);

		// ʹ��ʱ�����Ϊ������ӣ�ȷ��ÿ�βɼ����ݶ��б仯
		static unsigned int seedCounter = 0;
		srand(static_cast<unsigned int>(time(nullptr)) + (++seedCounter));

		// ��������
		for (unsigned long i = 0; i < size; ++i) {
			// ���㵱ǰ���ض�Ӧ�ĽǶȣ�0���У�
			double angle = (static_cast<double>(i) / static_cast<double>(size - 1)) * halfPeriod;

			// ����sinֵ
			double sinValue = std::sin(angle);

			// �����������������������
			int baseNoise = (rand() % (2 * noiseLevel + 1)) - noiseLevel;  // -noiseLevel �� +noiseLevel

																		   // ���������������sinֵ�����ϵ�С���Ȳ�����
			double jumpRange = 0.05;  // ������Χ������������5%
			double randomJump = ((double)rand() / RAND_MAX - 0.5) * 2.0 * jumpRange;  // -5% �� +5%

																					  // ���һЩ���仯��Ư�ƣ�ģ���¶�Ư�Ƶȣ�
			double driftPhase = (static_cast<double>(seedCounter) / 100.0) + (static_cast<double>(i) / static_cast<double>(size));
			double drift = sin(driftPhase) * actualAmplitude * 0.02;  // 2%��Ư��

																	  // ������������ֵ
			double pixelValue = baseValue + actualAmplitude * sinValue * (1.0 + randomJump) + baseNoise + drift;

			// ������16λ��Χ��
			if (pixelValue < 0) {
				pixelValue = 0;
			}
			else if (pixelValue > 65535) {
				pixelValue = 65535;
			}

			buff[i] = static_cast<unsigned short>(pixelValue);
		}

		// ģ���¶ȶ����ݵ�Ӱ�죨����������䣩
		if (m_coolingEnabled) {
			// �¶�Խ�ͣ�����ԽС���ź�����Խ��
			double tempFactor = 1.0 - (25.0 - m_exposureTime) / 25.0 * 0.1;  // ���10%�ĸ���

			for (unsigned long i = 0; i < size; ++i) {
				double adjustedValue = buff[i] * tempFactor;
				if (adjustedValue < 0) adjustedValue = 0;
				if (adjustedValue > 65535) adjustedValue = 65535;
				buff[i] = static_cast<unsigned short>(adjustedValue);
			}
		}

		// ��¼�ɼ��ɹ�
	//	LogPrintInfo("Virtual CCD: Generated {0} pixels of dynamic sin(x) half-period data with random variations", size);
		return true;

	}
	catch (const std::exception& e) {
		LogPrintErr("Exception in Virtual CCD DataAcqOneShot: {0}", std::string(e.what()));
		return false;
	}
	catch (...) {
		LogPrintErr("Unknown exception in Virtual CCD DataAcqOneShot");
		return false;
	}
}
