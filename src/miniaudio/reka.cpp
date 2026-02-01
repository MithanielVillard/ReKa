#include "reka.h"
#include "miniaudio/MiniAudioDevice.h"

#include <memory>

namespace reka 
{
	std::shared_ptr<Device> CreateDevice(DeviceConfig config)
	{
		return std::make_shared<MiniAudioDevice>(config);
	}
}
