#include "reka.h"
#include "PortAudioDevice.h"

#include <memory>

namespace reka 
{
	std::shared_ptr<Device> CreateDevice(DeviceConfig config)
	{
		return std::make_shared<PortAudioDevice>(config);
	}
}
