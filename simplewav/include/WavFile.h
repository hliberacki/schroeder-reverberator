#ifndef WAVFILE_H
#define WAVFILE_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <iomanip>

namespace wf {

enum class wfResult {
    SUCCESS = 0,
    FILE_NOT_FOUND,
    HEADER_READ_ERROR,
    SAMPLE_READ_ERROR,
    HEADER_MISMATCH
};

class WavFile {
public:
	WavFile();
    WavFile(std::string srcPath);
    ~WavFile();
    wfResult open(std::string srcPath);
    wfResult save(const std::vector<std::vector<int16_t>> &samples, uint32_t fs, std::string dstPath);
    wfResult save(const std::vector<int16_t> &samples, uint32_t fs, std::string dstPath);

    void SetChunkId(uint32_t chunkId);
    void SetChunkSize(uint32_t chunkSize);
    void SetFormat(uint32_t format);
    void SetSubchunk1Id(uint32_t subchunk1Id);
    void SetSubchunk1Size(uint32_t subchunk1Size);
    void SetAudioFormat(uint16_t audioFormat);
    void SetChannelsNum(uint16_t channelsNum);
    void SetSampleRate(uint32_t sampleRate);
    void SetByteRate(uint32_t byteRate);
    void SetBlockAlign(uint16_t blockAlign);
    void SetBitsPerSample(uint16_t bitsPerSample);
    void SetSubchunk2Id(uint32_t subchunk2Id);
    void SetSubchunk2Size(uint32_t subchunk2Size);

    uint32_t GetChunkId();
    uint32_t GetChunkSize();
    uint32_t GetFormat();
    uint32_t GetSubchunk1Id();
    uint32_t GetSubchunk1Size();
    uint16_t GetAudioFormat();
    uint16_t GetChannelsNum();
    uint32_t GetSampleRate();
    uint32_t GetByteRate();
    uint16_t GetBlockAlign();
    uint16_t GetBitsPerSample();
    uint32_t GetSubchunk2Id();
    uint32_t GetSubchunk2Size();
    uint64_t GetSamplesPerChannel();

    std::vector<std::vector<int16_t>> data;

private:
    bool loadHeader();
    bool testHeader();
    void processHeader();
    bool loadSamples();
    void constFillHeader();
    bool updateHeader();
    bool updateHeader(const std::vector<std::vector<int16_t>> &samples);
    bool updateHeader(const std::vector<int16_t> &samples);
    bool storeHeader();
    bool storeSamples(const std::vector<std::vector<int16_t>> &samples);
    bool storeSamples(const std::vector<int16_t> &samples);

	std::ifstream _srcFile;
	std::ofstream _dstFile;

    uint32_t _chunkId;
    uint32_t _chunkSize;
    uint32_t _format;
    uint32_t _subchunk1Id;
    uint32_t _subchunk1Size;
    uint16_t _audioFormat;
    uint16_t _channelsNum;
    uint32_t _sampleRate;
    uint32_t _byteRate;
    uint16_t _blockAlign;
    uint16_t _bitsPerSample;
    uint32_t _subchunk2Id;
    uint32_t _subchunk2Size;

    uint64_t _samplesPerChannel;
    double _lengthSeconds;
};

}

#endif
