#ifndef WAVEFORMAT__H_
#define WAVEFORMAT__H_

#include "types.h"

namespace reka 
{

enum class WAVE_AUDIO_FORMAT : uint16
{
	PCM_INTEGER = 1,
	IEEE_FLOAT = 3
};

struct WaveFormat
{
	WAVE_AUDIO_FORMAT audioFormat;
	int16 nbrChannel;
	int32 sampleRate;
	int16 bitsPerSample;
	int16 bytePerFrame; //nbrChannel * bitsPerSample / 8
	int32 bytePerSec; //bytePerFrame * sampleRate
};

}

#endif // !WAVEFORMAT__H_
