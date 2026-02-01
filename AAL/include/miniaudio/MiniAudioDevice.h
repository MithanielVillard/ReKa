#ifndef MINIAUDIODEV__H_
#define MINIAUDIODEV__H_

#include "Device.h"

#include <miniaudio.h>
namespace reka 
{


class MiniAudioDevice : public Device
{
public:
	MiniAudioDevice(DeviceConfig config);  
	~MiniAudioDevice() override;

	Error Init(DeviceConfig config) override;
	Error Uninit() override;

	void Start() override;
	void Stop() override ;

protected:
	int32 TranslateFormat(DeviceFormat format) override;

private:
	static void maStreamCallback(ma_device* device, void* output, void const* input,  ma_uint32 frames);

private:
	ma_device m_device;
};

}


#endif // !MINIAUDIODEV__H_
