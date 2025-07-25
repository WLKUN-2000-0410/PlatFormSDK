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

	// 检查设备是否连接
	if (!IsConnected()) {
		return false;
	}

	try {
		// 模拟数据采集延时（根据曝光时间）
		if (m_exposureTime > 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(m_exposureTime)));
		}

		// 生成sin(x)前1/2周期的模拟数据
		const double PI = 3.14159265358979323846;
		const double halfPeriod = PI;  // π 对应sin(x)的前1/2周期

									   // 基础参数
		const unsigned short baseValue = 1000;     // 基础偏移值
		const unsigned short amplitude = 30000;    // 振幅（适合16位数据范围）
		const unsigned short noiseLevel = 100;    // 噪声水平

												  // 增益影响振幅
		double gainFactor = static_cast<double>(m_gain) / 100.0;  // 假设增益基准为100
		unsigned short actualAmplitude = static_cast<unsigned short>(amplitude * gainFactor);

		// 使用时间戳作为随机种子，确保每次采集数据都有变化
		static unsigned int seedCounter = 0;
		srand(static_cast<unsigned int>(time(nullptr)) + (++seedCounter));

		// 生成数据
		for (unsigned long i = 0; i < size; ++i) {
			// 计算当前像素对应的角度（0到π）
			double angle = (static_cast<double>(i) / static_cast<double>(size - 1)) * halfPeriod;

			// 计算sin值
			double sinValue = std::sin(angle);

			// 生成随机噪声（基础噪声）
			int baseNoise = (rand() % (2 * noiseLevel + 1)) - noiseLevel;  // -noiseLevel 到 +noiseLevel

																		   // 生成随机跳动（在sin值基础上的小幅度波动）
			double jumpRange = 0.05;  // 跳动范围，相对于振幅的5%
			double randomJump = ((double)rand() / RAND_MAX - 0.5) * 2.0 * jumpRange;  // -5% 到 +5%

																					  // 添加一些慢变化的漂移（模拟温度漂移等）
			double driftPhase = (static_cast<double>(seedCounter) / 100.0) + (static_cast<double>(i) / static_cast<double>(size));
			double drift = sin(driftPhase) * actualAmplitude * 0.02;  // 2%的漂移

																	  // 计算最终像素值
			double pixelValue = baseValue + actualAmplitude * sinValue * (1.0 + randomJump) + baseNoise + drift;

			// 限制在16位范围内
			if (pixelValue < 0) {
				pixelValue = 0;
			}
			else if (pixelValue > 65535) {
				pixelValue = 65535;
			}

			buff[i] = static_cast<unsigned short>(pixelValue);
		}

		// 模拟温度对数据的影响（如果启用制冷）
		if (m_coolingEnabled) {
			// 温度越低，噪声越小，信号质量越好
			double tempFactor = 1.0 - (25.0 - m_temperature) / 25.0 * 0.1;  // 最多10%的改善

			for (unsigned long i = 0; i < size; ++i) {
				double adjustedValue = buff[i] * tempFactor;
				if (adjustedValue < 0) adjustedValue = 0;
				if (adjustedValue > 65535) adjustedValue = 65535;
				buff[i] = static_cast<unsigned short>(adjustedValue);
			}
		}
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
