#include "miniaudio/MiniAudioDevice.h"
#include "Device.h"
#include "Error.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>
namespace reka 
{

	MiniAudioDevice::MiniAudioDevice(DeviceConfig config) : Device(config)
	{
		Init(config);
	}

	MiniAudioDevice::~MiniAudioDevice()
	{
		Uninit();
	}

	int32 MiniAudioDevice::TranslateFormat(DeviceFormat const format)
	{
		switch (format) 
		{
			case DeviceFormat::INT16:
				return ma_format_s16;
			case DeviceFormat::INT24:
				return ma_format_s24;
			case DeviceFormat::INT32:
				return ma_format_s32;
			case DeviceFormat::FLOAT32:
				return ma_format_f32;
			default:
				return ma_format_f32;
		}
	}

	void MiniAudioDevice::maStreamCallback(ma_device* device, void* output, void const* input,  ma_uint32 frames)
	{
		MiniAudioDevice* pDevice = reinterpret_cast<MiniAudioDevice*>(device->pUserData);
		pDevice->m_config.pAudioCallback(pDevice, input, output, frames);
	}

	Error MiniAudioDevice::Init(DeviceConfig config)
	{
	    ma_device_config maconf = ma_device_config_init(ma_device_type_playback);
	    maconf.playback.format   = static_cast<ma_format>(TranslateFormat(config.format));
	    maconf.playback.channels = config.channels;
	    maconf.sampleRate        = config.sampleRate;
	    maconf.dataCallback      = maStreamCallback;
	    maconf.pUserData         = this;

		m_config = config;

    	if (ma_device_init(NULL, &maconf, &m_device) != MA_SUCCESS) 
		{
			return {REKA_FAILED, "Failed to initialize MiniAudio"};
   	 	}
		
		return {REKA_OK, ""};
	}

	Error MiniAudioDevice::Uninit()
	{
		ma_device_uninit(&m_device);
		return {REKA_OK, ""};
	}


	void MiniAudioDevice::Start()
	{
		ma_device_start(&m_device);
	}

	void MiniAudioDevice::Stop()
	{
		ma_device_stop(&m_device);
	}

}
