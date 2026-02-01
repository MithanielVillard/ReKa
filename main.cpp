#include "Device.h"
#include "reka.h"
#include "types.h"

#include <iostream>

void ProcessCallback(reka::Device* device, void const* input, void* output, uint64 frames)
{
	std::cout << "Hello world\n";
}

int main(int argc, char **argv) 
{
	reka::DeviceConfig config;
	config.channels = 2;
	config.format = reka::DeviceFormat::FLOAT32;
	config.sampleRate = 48000;
	config.pAudioCallback = ProcessCallback;
	std::shared_ptr<reka::Device> device = reka::CreateDevice(config);
	device->Start();

    return 0;
}
