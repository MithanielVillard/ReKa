#ifndef DEVICE__H_
#define DEVICE__H_

#include "Error.h"
#include "types.h"

#include <functional>

namespace reka
{

class Device;
using AudioProcessCallback = std::function<void(Device* pDevice, void const* input, void* output, uint64 framesCount)>;

enum class DeviceFormat
{
	INT16,
	INT24,
	INT32,
	FLOAT32
};

struct DeviceConfig
{
	DeviceFormat format;
	int16 channels;
	int32 sampleRate;
	AudioProcessCallback pAudioCallback;
	void* pCustomData;
};


class Device
{
public:
	Device() = default;
	Device(DeviceConfig config) : m_config(config) {};
	Device(Device const& other) = delete;
	Device(Device && other) noexcept = delete;
	
	Device& operator=(Device const& other) = delete;
	Device& operator=(Device && other) noexcept = delete;
	virtual ~Device() = default;

	virtual Error Init(DeviceConfig config) = 0;
	virtual	Error Uninit() = 0;

	virtual void Start() = 0;
	virtual void Stop() = 0;

	void* GetUserData() const { return m_config.pCustomData; }

protected:
	virtual int32 TranslateFormat(DeviceFormat format) = 0;

protected:
	DeviceConfig m_config;
};

}
#endif // !DEVICE__H_
