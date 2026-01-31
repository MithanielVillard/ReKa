#ifndef REKA__H_
#define REKA__H_

#include "Device.h"

#include <memory>
namespace reka 
{
	std::shared_ptr<Device> CreateDevice(DeviceConfig config);
}

#endif // !REKA__H_
