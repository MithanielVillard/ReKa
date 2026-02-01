#ifndef PORTAUDIODEV__H_
#define PORTAUDIODEV__H_

#include "Device.h"

#include <portaudio.h>
namespace reka 
{


class PortAudioDevice : public Device
{
public:
	PortAudioDevice(DeviceConfig config);  
	~PortAudioDevice() override;

	Error Init(DeviceConfig config) override;
	Error Uninit() override;

	void Start() override;
	void Stop() override ;

protected:
	int32 TranslateFormat(DeviceFormat format) override;

private:
	static int32 paStreamCallback(const void* input, void* output, unsigned long frames, 
	const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData);

private:
	PaStream* m_pStream = nullptr;
};

}
#endif // !PORTAUDIODEV__H_
