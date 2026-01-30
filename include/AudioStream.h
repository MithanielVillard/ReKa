#ifndef AUDIOSTREAM__H_
#define AUDIOSTREAM__H_

#include "WaveFormat.h"
#include "types.h"

#include <fstream>
#include <string>

namespace reka 
{

#pragma pack(push, 1)
struct RIFFHeader
{
    char8   ckID[4];        // "RIFF"
    uint32  ckSize;
    char8   WAVEID[4];      // "WAVE"
};

struct FmtChunk
{
    char8   ckID[4];        // "fmt "
    uint32  ckSize;
    uint16  wFormatTag;
    uint16  nChannels;
    uint32  nSamplesPerSec;
    uint32  nAvgBytesPerSec;
    uint16  nBlockAlign;
    uint16  wBitsPerSample;
};
#pragma pack(pop)


class AudioStream
{
public:
    AudioStream() = default;
    explicit AudioStream(std::string const& path);
    AudioStream(AudioStream&& other) noexcept = default;


    AudioStream& operator=(AudioStream&& other) noexcept = default;
    ~AudioStream();


    bool                     Open(std::string const& path);
    void       			     Stream(char8* outBuff, int32 startFrame, int32 frameToStream, int32 channel = 0);
    void                     StreamInterlaced(char8* outBuff, int32 startFrame, int32 frameToStream);
    void				     StreamFromEnd(char8* outBuff, int32 startFrame, int32 frameToStream, int32 channel = 0);
    void                     StreamInterlacedFromEnd(char8* outBuff, int32 startFrame, int32 frameToStream);

    uint32                   GetDataSize();
	WaveFormat const& 		 GetWaveFormat() const { return m_waveFormat; }

private:
    RIFFHeader               m_header;
    FmtChunk                 m_fmt;
    bool                     m_fmtValid = false;

    std::ifstream            m_file;
    std::streampos           m_dataStartPos {0};
    uint32                   m_dataSize = 0;

	WaveFormat				 m_waveFormat;

	static constexpr uint32  FmtChunkSize = 16;

private:
    bool                     ReadRIFFHeader();
    bool                     FindChunk(const char id[4], uint32& chunkSize);
    bool                     ReadFmtChunk(uint32 chunkSize);
    bool                     LocateDataChunk();

    int32                    GetValidFrameCount(int32 startFrame, int32 requestedCount) const;
    void                     Display() const;
};

}
#endif // !AUDIOSTREAMER__H_
