#include "AudioStream.h"
#include "WaveFormat.h"
#include "types.h"

#include <fstream>
#include <iostream>
#include <cstring>

namespace reka 
{

static bool IdEquals(char8 a[4], const char b[5])
{
    return std::memcmp(a, b, 4) == 0;
}

AudioStream::AudioStream(std::string const& path)
{
    if (!Open(path))
        std::cerr << "Failed to initialize SoundStream from path: " << path << "\n";
}

AudioStream::~AudioStream() 
{
    m_file.close();
}

bool AudioStream::Open(std::string const &path)
{
    if (!m_file.is_open())
        m_file.open(path, std::ios::binary);


    if (!m_file.is_open())
    {
        std::cerr << "Unable to open the file : " << path << '\n';
        return false;
    }

    if (!ReadRIFFHeader()) return false;

    uint32 chunkSize = 0;

    m_file.seekg(12, std::ios::beg);

    if (!FindChunk("fmt ", chunkSize))      	  return false;
    if (!ReadFmtChunk(chunkSize))                 return false;
    if (!LocateDataChunk())                       return false;

    Display();
    return true;
}


//////////////////////////
///     Stream Zone    ///
//////////////////////////
void AudioStream::Stream(char8* outBuff, int32 startFrame, int32 frameToStream, int32 channel)
{
    if (channel < 0 || channel >= static_cast<int32>(m_fmt.nChannels)) return;
    if (!m_file.is_open()) return;

    int32 count = GetValidFrameCount(startFrame, frameToStream);
    if (count == 0) return;

    int32 blockAlign = m_fmt.nBlockAlign;
    int32 bytesPerSample = m_fmt.wBitsPerSample / 8;

    int32 bytesToRead = count * blockAlign;
    char8* tempBuffer = new char8[bytesToRead];

    std::streampos readPos = m_dataStartPos + static_cast<std::streamoff>(startFrame * blockAlign);

    m_file.seekg(readPos);
    m_file.read(reinterpret_cast<char*>(tempBuffer), bytesToRead);

    if (!m_file) return;

    int32 channelOffset = channel * bytesPerSample;
    int32 cursor = channelOffset;

    for (int32 i = 0; i < count; ++i)
    {
        for (int32 b = 0; b < bytesPerSample; ++b)
        {
            outBuff[cursor + b] = tempBuffer[cursor + b];
        }
        cursor += blockAlign;
    }

	delete [] tempBuffer;
}

void AudioStream::StreamInterlaced(char8* outBuff, int32 startFrame, int32 frameToStream)
{
    if (!m_file.is_open()) return;

    int32 count = GetValidFrameCount(startFrame, frameToStream);
    if (count == 0) return;

    int32 byteCount = count * m_fmt.nBlockAlign;

    std::streampos readPos = 0 + static_cast<std::streamoff>(startFrame * m_fmt.nBlockAlign);

    m_file.seekg(readPos);
    m_file.read(reinterpret_cast<char*>(outBuff), byteCount);
}


void AudioStream::StreamFromEnd(char8* outBuff, int32 startFrame, int32 frameToStream, int32 channel)
{
    if (channel < 0 || channel >= static_cast<int32>(m_fmt.nChannels)) return;
    if (!m_file.is_open()) return;

    int32 count = GetValidFrameCount(startFrame, frameToStream);
    if (count == 0) return;

    int32 blockAlign = m_fmt.nBlockAlign;
    int32 bytesPerSample = m_fmt.wBitsPerSample / 8;

    int32 bytesToRead = count * blockAlign;
    char8* tempBuffer = new char8[bytesToRead];

    std::streampos readPos = m_dataSize - static_cast<std::streamoff>((startFrame + 1) * m_fmt.nBlockAlign);

    m_file.seekg(readPos);
    m_file.read(reinterpret_cast<char*>(tempBuffer), bytesToRead);

    int32 channelOffset = channel * bytesPerSample;
    int32 cursor = channelOffset;

    for (int32 i = 0; i < count; ++i)
    {
        for (int32 b = 0; b < bytesPerSample; ++b)
        {
            outBuff[cursor + b] = tempBuffer[cursor + b];
        }
        cursor += blockAlign;
    }

	delete [] tempBuffer;
}

void AudioStream::StreamInterlacedFromEnd(char8 *outBuff, int32 startFrame, int32 frameToStream)
{
    if (!m_file.is_open()) return;

    int32 count = GetValidFrameCount(startFrame, frameToStream);
    if (count == 0) return;

    int32 byteCount = count * m_fmt.nBlockAlign;

    std::streampos readPos = m_dataSize - ((startFrame + count) * m_fmt.nBlockAlign);

    m_file.clear();
    m_file.seekg(readPos);
    m_file.read(reinterpret_cast<char*>(outBuff), byteCount);
}

int32 AudioStream::GetValidFrameCount(int32 startFrame, int32 requestedCount) const
{
    if (!m_fmtValid || m_dataSize == 0 || m_fmt.nBlockAlign == 0) return 0;

    int32 totalFrames = static_cast<int32>(m_dataSize) / m_fmt.nBlockAlign;

    if (startFrame < 0 || startFrame >= totalFrames) return 0;

    int32 count = requestedCount;
    if (startFrame + count > totalFrames)
        count = totalFrames - startFrame;

    return count;
}

uint32 AudioStream::GetDataSize()
{
    return m_dataSize;
}

/////////////////////////////
///     Read checking     ///
/////////////////////////////
bool AudioStream::ReadRIFFHeader()
{
    m_file.read(reinterpret_cast<char*>(&m_header), sizeof(m_header));
    if (!m_file) return false;
    return (IdEquals(m_header.ckID, "RIFF") && IdEquals(m_header.WAVEID, "WAVE"));
}

bool AudioStream::FindChunk(const char id[4], uint32& chunkSize)
{
    while (m_file)
    {
        char8 curId[4];
        m_file.read(reinterpret_cast<char*>(curId), 4);
        if (!m_file) return false;

        uint32 size = 0;
        m_file.read(reinterpret_cast<char*>(&size), sizeof(size));
        if (!m_file) return false;

        if (IdEquals(curId, id))
        {
            chunkSize = size;
            return true;
        }

        m_file.seekg(size, std::ios::cur);
		//TODO why ?
        if (size % 2 == 1) m_file.seekg(1, std::ios::cur);
    }
    return false;
}

bool AudioStream::ReadFmtChunk(uint32 chunkSize)
{
    if (chunkSize < FmtChunkSize) return false;
    std::memcpy(m_fmt.ckID, "fmt ", 4);
    m_fmt.ckSize = chunkSize;

	m_file.read(reinterpret_cast<char*>(&m_fmt.wFormatTag), sizeof(FmtChunk) - 4 * sizeof(char8));

    m_waveFormat.audioFormat      = static_cast<WAVE_AUDIO_FORMAT>(m_fmt.wFormatTag);
    m_waveFormat.nbrChannel       = m_fmt.nChannels;
	m_waveFormat.sampleRate       = m_fmt.nSamplesPerSec;
    m_waveFormat.bytePerSec       = m_fmt.nAvgBytesPerSec;
    m_waveFormat.bytePerFrame     = m_fmt.nBlockAlign;
    m_waveFormat.bitsPerSample    = m_fmt.wBitsPerSample;

    int64 remaining = chunkSize - 16;
    if (remaining > 0) m_file.seekg(remaining, std::ios::cur);

    if (!m_file) return false;
    m_fmtValid = true;
    return true;
}

bool AudioStream::LocateDataChunk()
{
    m_file.seekg(12, std::ios::beg);

    uint32 chunkSize = 0;
    if (!FindChunk("data", chunkSize)) return false;
    if (chunkSize == 0) return false;

    m_dataSize = chunkSize;
    m_dataStartPos = m_file.tellg();

    return true;
}

/////////////////////
///     Debug     ///
/////////////////////
void AudioStream::Display() const
{
    std::cout << "--- HEADER ---\n";
    std::cout << "ckId : "                          << std::string(reinterpret_cast<const char*>(m_header.ckID), 4)    << '\n';
    std::cout << "ckSize : "                        << m_header.ckSize                        						   << '\n';
    std::cout << "WAVEID : "                        << std::string(reinterpret_cast<const char*>(m_header.WAVEID), 4)  << '\n';

    if (m_fmtValid)
    {
        std::cout << "\n--- FORMAT (fmt ) ---\n";
        std::cout << "ckId : "                      << std::string(reinterpret_cast<const char*>(m_fmt.ckID), 4)       << '\n';
        std::cout << "ckSize : "                    << m_fmt.ckSize                          						   << '\n';
        std::cout << "wFormatTag : "                << m_fmt.wFormatTag                      						   << '\n';
        std::cout << "nChannels : "                 << m_fmt.nChannels                       						   << '\n';
        std::cout << "nSamplesPerSec : "            << m_fmt.nSamplesPerSec                  						   << '\n';
        std::cout << "nAvgBytesPerSec : "           << m_fmt.nAvgBytesPerSec                 						   << '\n';
        std::cout << "nBlockAlign : "               << m_fmt.nBlockAlign                     						   << '\n';
        std::cout << "wBitsPerSample : "            << m_fmt.wBitsPerSample                  						   << '\n';
    }
    else
        std::cout << "(Unread fmt chunk.)\n";
}

}
