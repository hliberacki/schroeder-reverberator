    #include "../include/WavFile.h"

using namespace wf;

WavFile::WavFile() {
	std::cout << "WavFile::c'tor" << std::endl;
    //data.resize(0, std::vector<int16_t>(0, 0));
    //std::cout << "sizeof(data): " << sizeof(data) << std::endl;
}

WavFile::WavFile(std::string srcPath) {
    std::cout << "WavFile::c'tor" << std::endl;
    wfResult result = open(srcPath);
    if(result != wfResult::SUCCESS) {
        std::cout << "Error: " << static_cast<uint32_t>(result) << std::endl;
    }
    //_data.resize(0, std::vector<int16_t>(0, 0));
    //std::cout << "sizeof(data): " << sizeof(data) << std::endl;
}

WavFile::~WavFile() {
    std::cout << "WavFile::d'tor" << std::endl;
    _srcFile.close();
}

wfResult WavFile::open(std::string srcPath) {
    std::cout << "WavFile::open" << std::endl;

    _srcFile.open(srcPath.c_str(), std::ios::binary);

    if (!_srcFile.is_open()) {
        std::cout << "Error while opening source file" << std::endl;
        return wfResult::FILE_NOT_FOUND;
    }

    if(!loadHeader()) {
        std::cout << "Error while reading header" << std::endl;
        return wfResult::HEADER_READ_ERROR;
    }

    if(!testHeader()) {
        std::cout << "Header mismatch (not a wav file?)" << std::endl;
        return wfResult::HEADER_MISMATCH;
    }

    // fill additional variables like samples per channel etc
    processHeader();

    if(!loadSamples()) {
        std::cout << "Error while reading samples" << std::endl;
        return wfResult::SAMPLE_READ_ERROR;
    }

    return wfResult::SUCCESS;
}

wfResult WavFile::save(const std::vector<std::vector<int16_t>> &samples, uint32_t fs, std::string dstPath) {
    _dstFile.open(dstPath.c_str(), std::ios::binary | std::ios::trunc);
    _sampleRate = fs;
    updateHeader(samples);
    storeHeader();
    storeSamples(samples);
}

wfResult WavFile::save(const std::vector<int16_t> &samples, uint32_t fs, std::string dstPath) {
    _dstFile.open(dstPath.c_str(), std::ios::binary | std::ios::trunc);
    _sampleRate = fs;
    updateHeader(samples);
    storeHeader();
    storeSamples(samples);
}

void WavFile::SetChunkId(uint32_t chunkId) {
    _chunkId = chunkId;
}

void WavFile::SetChunkSize(uint32_t chunkSize) {
    _chunkSize = chunkSize;
}

void WavFile::SetFormat(uint32_t format) {
    _format = format;
}

void WavFile::SetSubchunk1Id(uint32_t subchunk1Id) {
    _subchunk1Id = subchunk1Id;
}

void WavFile::SetSubchunk1Size(uint32_t subchunk1Size) {
    _subchunk1Size = subchunk1Size;
}

void WavFile::SetAudioFormat(uint16_t audioFormat) {
    _audioFormat = audioFormat;
}

void WavFile::SetChannelsNum(uint16_t channelsNum) {
    _channelsNum = channelsNum;
}

void WavFile::SetSampleRate(uint32_t sampleRate) {
    _sampleRate = sampleRate;
}

void WavFile::SetByteRate(uint32_t byteRate) {
    _byteRate = byteRate;
}

void WavFile::SetBlockAlign(uint16_t blockAlign) {
    _blockAlign = blockAlign;
}

void WavFile::SetBitsPerSample(uint16_t bitsPerSample) {
    _bitsPerSample = bitsPerSample;
}

void WavFile::SetSubchunk2Id(uint32_t subchunk2Id) {
    _subchunk2Id = subchunk2Id;
}

void WavFile::SetSubchunk2Size(uint32_t subchunk2Size) {
    _subchunk2Size = subchunk2Size;
}

uint32_t WavFile::GetChunkId() {
    return _chunkId;
}

uint32_t WavFile::GetChunkSize() {
    return _chunkSize;
}

uint32_t WavFile::GetFormat() {
    return _format;
}

uint32_t WavFile::GetSubchunk1Id() {
    return _subchunk1Id;
}

uint32_t WavFile::GetSubchunk1Size() {
    return _subchunk1Size;
}

uint16_t WavFile::GetAudioFormat() {
    return _audioFormat;
}

uint16_t WavFile::GetChannelsNum() {
    return _channelsNum;
}

uint32_t WavFile::GetSampleRate() {
    return _sampleRate;
}

uint32_t WavFile::GetByteRate() {
    return _byteRate;
}

uint16_t WavFile::GetBlockAlign() {
    return _blockAlign;
}

uint16_t WavFile::GetBitsPerSample() {
    return _bitsPerSample;
}

uint32_t WavFile::GetSubchunk2Id() {
    return _subchunk2Id;
}

uint32_t WavFile::GetSubchunk2Size() {
    return _subchunk2Size;
}

uint64_t WavFile::GetSamplesPerChannel() {
    return _samplesPerChannel;
}

bool WavFile::loadHeader() {
    _srcFile.read(reinterpret_cast<char *>(&_chunkId), sizeof(_chunkId));
    _srcFile.read(reinterpret_cast<char *>(&_chunkSize), sizeof(_chunkSize));
    _srcFile.read(reinterpret_cast<char *>(&_format), sizeof(_format));
    _srcFile.read(reinterpret_cast<char *>(&_subchunk1Id), sizeof(_subchunk1Id));
    _srcFile.read(reinterpret_cast<char *>(&_subchunk1Size), sizeof(_subchunk1Size));
    _srcFile.read(reinterpret_cast<char *>(&_audioFormat), sizeof(_audioFormat));
    _srcFile.read(reinterpret_cast<char *>(&_channelsNum), sizeof(_channelsNum));
    _srcFile.read(reinterpret_cast<char *>(&_sampleRate), sizeof(_sampleRate));
    _srcFile.read(reinterpret_cast<char *>(&_byteRate), sizeof(_byteRate));
    _srcFile.read(reinterpret_cast<char *>(&_blockAlign), sizeof(_blockAlign));
    _srcFile.read(reinterpret_cast<char *>(&_bitsPerSample), sizeof(_bitsPerSample));
    _srcFile.read(reinterpret_cast<char *>(&_subchunk2Id), sizeof(_subchunk2Id));
    _srcFile.read(reinterpret_cast<char *>(&_subchunk2Size), sizeof(_subchunk2Size));

    if(_srcFile) {
        return true;
    } else {
        // _srcFile.gcount() - bytes read before failure, but it should be checked after each reading
        return false;
    }
}

bool WavFile::testHeader() {
    bool retVal = true;

    // const fields check
    if(_chunkId != 1179011410U) retVal = false;
    if(_subchunk1Id != 544501094U) retVal = false;
    if(_subchunk2Id != 1635017060U) retVal = false;

    if(_audioFormat != 1) retVal = false; // audio format different than 1 indicate compression

    // TODO: perform more checks based on chunk ane file sizes

    return retVal;
}

void WavFile::processHeader() {
    _samplesPerChannel = _subchunk2Size/(_channelsNum*(_bitsPerSample/8));
    std::cout << "_samplesPerChannel: " << _samplesPerChannel << std::endl;

    _lengthSeconds = _samplesPerChannel/static_cast<double>(_sampleRate);
    std::cout << std::fixed << std::setprecision(3) << "_lengthSeconds: " << _lengthSeconds << std::endl;

}

bool WavFile::loadSamples() {
    // reserve the memory for the samples vector
    data.resize(_channelsNum, std::vector<int16_t> (_samplesPerChannel, 0)); //TODO!!!!!!!

    for(uint64_t sampleIter = 0; sampleIter < _samplesPerChannel; ++sampleIter)
    {
        for(uint16_t channelIter = 0; channelIter < GetChannelsNum(); ++channelIter)
        {
            int16_t sampleBuff;
            _srcFile.read(reinterpret_cast<char *>(&sampleBuff), sizeof(sampleBuff));
            data[channelIter][sampleIter] = sampleBuff;
            //std::cout << sampleBuff;
            // sampleFloat = sampleInt / 32768.0f; //(0x8000)
        }
    }
    return true;
}

void WavFile::constFillHeader() {
    _chunkId = 1179011410U; //make them static consts or constexpr
    _format = 1163280727U;
    _subchunk1Id = 544501094U;
    _subchunk1Size = 16U;
    _audioFormat = 1U;
    _bitsPerSample = 16U;
    _subchunk2Id = 1635017060U;
}

bool WavFile::updateHeader() {
    constFillHeader();
    _channelsNum = data.size();
    _byteRate = _sampleRate * _channelsNum * (_bitsPerSample/8);
    _blockAlign = _channelsNum * (_bitsPerSample/8);
    _samplesPerChannel = data[0].size();

    _subchunk2Size = _samplesPerChannel * _channelsNum * (_bitsPerSample / 8);
    _chunkSize = 36U + _subchunk2Size;
}

bool WavFile::updateHeader(const std::vector<std::vector<int16_t>> &samples) {
    constFillHeader();
    _channelsNum = samples.size();
    _byteRate = _sampleRate * _channelsNum * (_bitsPerSample/8);
    _blockAlign = _channelsNum * (_bitsPerSample/8);
    _samplesPerChannel = samples[0].size();

    _subchunk2Size = _samplesPerChannel * _channelsNum * (_bitsPerSample / 8);
    _chunkSize = 36U + _subchunk2Size;
}

bool WavFile::updateHeader(const std::vector<int16_t> &samples) {
    constFillHeader();
    _channelsNum = 1U;
    _byteRate = _sampleRate * _channelsNum * (_bitsPerSample/8);
    _blockAlign = _channelsNum * (_bitsPerSample/8);
    _samplesPerChannel = samples.size();

    _subchunk2Size = _samplesPerChannel * _channelsNum * (_bitsPerSample / 8);
    _chunkSize = 36U + _subchunk2Size;
}

bool WavFile::storeHeader() {
    _dstFile.write(reinterpret_cast<char *>(&_chunkId), sizeof(_chunkId));
    _dstFile.write(reinterpret_cast<char *>(&_chunkSize), sizeof(_chunkSize));
    _dstFile.write(reinterpret_cast<char *>(&_format), sizeof(_format));
    _dstFile.write(reinterpret_cast<char *>(&_subchunk1Id), sizeof(_subchunk1Id));
    _dstFile.write(reinterpret_cast<char *>(&_subchunk1Size), sizeof(_subchunk1Size));
    _dstFile.write(reinterpret_cast<char *>(&_audioFormat), sizeof(_audioFormat));
    _dstFile.write(reinterpret_cast<char *>(&_channelsNum), sizeof(_channelsNum));
    _dstFile.write(reinterpret_cast<char *>(&_sampleRate), sizeof(_sampleRate));
    _dstFile.write(reinterpret_cast<char *>(&_byteRate), sizeof(_byteRate));
    _dstFile.write(reinterpret_cast<char *>(&_blockAlign), sizeof(_blockAlign));
    _dstFile.write(reinterpret_cast<char *>(&_bitsPerSample), sizeof(_bitsPerSample));
    _dstFile.write(reinterpret_cast<char *>(&_subchunk2Id), sizeof(_subchunk2Id));
    _dstFile.write(reinterpret_cast<char *>(&_subchunk2Size), sizeof(_subchunk2Size));
}

bool WavFile::storeSamples(const std::vector<std::vector<int16_t>> &samples) {
    std::cout << "samples.size(): " << samples.size() << std::endl;
    std::cout << "samples[0].size(): " << samples[0].size() << std::endl;
    for(uint64_t sampleIter = 0; sampleIter < _samplesPerChannel; ++sampleIter)
    {
        for(uint16_t channelIter = 0; channelIter < GetChannelsNum(); ++channelIter)
        {
            int16_t sampleBuff = samples[channelIter][sampleIter];
            //std::cout << sampleBuff;
            _dstFile.write(reinterpret_cast<char *>(&sampleBuff), sizeof(sampleBuff));
        }
    }
}

bool WavFile::storeSamples(const std::vector<int16_t> &samples) {
    std::cout << "samples.size(): " << samples.size() << std::endl;
    for(uint64_t sampleIter = 0; sampleIter < _samplesPerChannel; ++sampleIter)
    {
        int16_t sampleBuff = samples[sampleIter];
        //std::cout << sampleBuff;
        _dstFile.write(reinterpret_cast<char *>(&sampleBuff), sizeof(sampleBuff));
    }
}
