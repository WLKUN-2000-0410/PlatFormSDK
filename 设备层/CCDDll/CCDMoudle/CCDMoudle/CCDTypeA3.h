#pragma once
#include "CCDInterface.h"
#include <Windows.h>
typedef enum _READOUT_MODE_ {
	mode_fvb = 0,
	mode_image,
} readout_mode;

class CCDTypeA3 : public CCDBase
{
public:
	CCDTypeA3();
	~CCDTypeA3();

public:
	bool   Initialize(const CCDConfig& config) override;
	bool   IsConnected() override;
	bool   Connect() override;
	bool   DisConnect() override;
	bool   SetExposureTime(double timeMs) override;
	bool   GetExposureTime(double* timeMs) override;
	bool   GetPixelNum(int* size) override;
	bool   DataAcqOneShot(unsigned short* buff, unsigned long size) override;
	bool   SetGain(int gain) override;
	bool   GetGain(int* gain) override;
	bool   EnableCooling(bool enable) override;
	bool   SetCoolingTemperature(double temp) override;
	bool   GetCurrentTemperature(double* temp) override;

private:
	bool initDll();
	void loadDllFun();

private:
	HINSTANCE m_hModule;    //windows.h  Handle to the loaded dfield.dll

	bool      m_isConnected;
	char      m_deviceId;     //The dfield API uses a 'char' as a device handle
	int       m_pixelNum;

	// === Function Pointers for dfield.dll ===
	typedef char(*dfield_open_t)(const char*);
	typedef void(*dfield_close_t)(char);
	typedef bool(*dfield_is_open_t)(char);
	typedef void(*dfield_get_detector_size_t)(char, short*, short*);
	typedef bool(*dfield_set_exposure_t)(char, int);
	typedef bool(*dfield_get_exposure_t)(char, int*);
	typedef bool(*dfield_capture_t)(char);
	typedef bool(*dfield_get_data_t)(char, unsigned short*, int);
	typedef bool(*dfield_set_gain_t)(char, short);
	typedef bool(*dfield_get_gain_t)(char, short*);
	typedef bool(*dfield_start_cooling_t)(char);
	typedef bool(*dfield_stop_cooling_t)(char);
	typedef bool(*dfield_set_temperature_t)(char, float);
	typedef bool(*dfield_get_real_temperature_t)(char, float*);
	typedef bool(*dfield_usb_mode_t)(bool);
	typedef bool(*dfield_get_bin_t)(char,short*,short*);
	typedef bool (*dfield_set_readout_mode)(char id, readout_mode mode);

	dfield_open_t                   m_dfield_open;
	dfield_close_t                  m_dfield_close;
	dfield_is_open_t                m_dfield_is_open;
	dfield_get_detector_size_t      m_dfield_get_detector_size;
	dfield_set_exposure_t           m_dfield_set_exposure;
	dfield_get_exposure_t           m_dfield_get_exposure;
	dfield_capture_t                m_dfield_capture;
	dfield_get_data_t               m_dfield_get_data;
	dfield_set_gain_t               m_dfield_set_gain;
	dfield_get_gain_t               m_dfield_get_gain;
	dfield_start_cooling_t          m_dfield_start_cooling;
	dfield_stop_cooling_t           m_dfield_stop_cooling;
	dfield_set_temperature_t        m_dfield_set_temperature;
	dfield_get_real_temperature_t   m_dfield_get_real_temperature;
	dfield_usb_mode_t               m_dfield_usb_mode;
	dfield_get_bin_t                m_dfield_get_bin;
	dfield_set_readout_mode         m_dfield_set_readout_mode;
};