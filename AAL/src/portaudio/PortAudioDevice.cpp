#include "portaudio/PortAudioDevice.h"
#include "Device.h"
#include "types.h"

#include <portaudio.h>
namespace reka
{
	PortAudioDevice::PortAudioDevice(DeviceConfig config) : Device(config)
	{
		Init(config);
	}

	PortAudioDevice::~PortAudioDevice()
	{
		Uninit();
	}

	int32 PortAudioDevice::TranslateFormat(DeviceFormat const format)
	{
		switch (format) 
		{
			case DeviceFormat::INT16:
				return paInt16;
			case DeviceFormat::INT24:
				return paInt24;
			case DeviceFormat::INT32:
				return paInt32;
			case DeviceFormat::FLOAT32:
				return paFloat32;
			default:
				return paFloat32;
		}
	}
	
	int32 PortAudioDevice::paStreamCallback(const void* input, void* output, unsigned long frames, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
	{
		PortAudioDevice* pDevice = reinterpret_cast<PortAudioDevice*>(userData);
		pDevice->m_config.pAudioCallback(pDevice, input, output, static_cast<uint64>(frames));
		return 0;	
	}

	Error PortAudioDevice::Init(DeviceConfig config)
	{
		PaError err = Pa_Initialize();
		if(err != paNoError)
			return {REKA_FAILED, "Failed to initialize Portaudio : " + std::string(Pa_GetErrorText(err), 34)};


		m_config = config;

		err = Pa_OpenDefaultStream(&m_pStream, 0, 2, 
			static_cast<PaSampleFormat>(TranslateFormat(config.format)), config.sampleRate, 256, &paStreamCallback, this);
		if(err != paNoError)
			return {REKA_FAILED, "Failed to open Portaudio stream : " + std::string(Pa_GetErrorText(err), 34)};

		return {REKA_OK, ""};
	}


	Error PortAudioDevice::Uninit()
	{
		PaError err = Pa_Terminate();
		if(err != paNoError)
			return {REKA_FAILED, "Failed to uninit Portaudio stream : " + std::string(Pa_GetErrorText(err), 34)};
		
		return {REKA_OK, ""};
	}


	void PortAudioDevice::Start()
	{
		PaError err = Pa_StartStream(m_pStream);
	}

	void PortAudioDevice::Stop()
	{
		PaError err = Pa_StopStream(m_pStream);
	}
}

