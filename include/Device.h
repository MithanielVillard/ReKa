#ifndef DEVICE__H_
#define DEVICE__H_

#include "Error.h"
#include "WaveFormat.h"

#include <functional>

namespace reka
{

using AudioProcessCallback = std::function<void(float32 const* input, float32* output, float32 framesCount, void* customData)>;

struct DeviceConfig
{
	WAVE_AUDIO_FORMAT format;
	int16 channels;
	int32 sampleRate;
	AudioProcessCallback pAudioCallback;
	void* pCustomData;
};


//Abstract device class (called a stream in port audio)
class Device
{
public:
	Device() = default;
	Device(DeviceConfig config);
	Device(Device const& other) = delete;
	Device(Device && other) noexcept = delete;
	
	Device& operator=(Device const& other) = delete;
	Device& operator=(Device && other) noexcept = delete;
	virtual ~Device() = default;

	virtual ErrorStatus Init(DeviceConfig config) = 0;
	virtual ErrorStatus Uninit() = 0;

	virtual void Start() = 0;
	virtual void Stop() = 0;
};

}
#endif // !DEVICE__H_
