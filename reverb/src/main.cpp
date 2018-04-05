#include <iostream>
#include <vector>
#include <cstdlib>
#include "WavFile.h"

class APFilter {
public:
    APFilter(float g, int k) {
        m_gain = g;
        m_buffer.resize(k, 0);
        begin_pos = 0;
        end_pos = k-1;
    }
    ~APFilter() {
        std::cout << "ap dtor" << std::endl;
    }
    int16_t process(const int16_t &input) {
        m_buffer[end_pos] = input - (input * m_gain);
        int16_t output = (m_gain * (input + m_buffer[begin_pos])) + m_buffer[begin_pos];
        updatePositions();
        return output;
    }
private:
    void updatePositions() {
        if(begin_pos+1 <= m_buffer.size()) {
            ++begin_pos;
        } else {
            begin_pos = 0;
        }
        if(end_pos+1 <= m_buffer.size()) {
            ++end_pos;
        } else {
            end_pos = 0;
        }
    }
    std::vector<int16_t> m_buffer;
    float m_gain;
    int begin_pos;
    int end_pos;
};

class CombFilter {
public:
    CombFilter(float g, int k) {
        m_gain = g;
        m_buffer.resize(k, 0);
        begin_pos = 0;
        end_pos = k-1;
    }
    ~CombFilter() {
        std::cout << "comb dtor" << std::endl;
    }
    int16_t process(const int16_t &input) {
        m_buffer[end_pos] = input;
        int16_t output = input + (m_gain * m_buffer[begin_pos]);
        updatePositions();
        return output;
    }
private:
    void updatePositions() {
        if(begin_pos+1 <= m_buffer.size()) {
            ++begin_pos;
        } else {
            begin_pos = 0;
        }
        if(end_pos+1 <= m_buffer.size()) {
            ++end_pos;
        } else {
            end_pos = 0;
        }
    }
    std::vector<int16_t> m_buffer;
    float m_gain;
    int begin_pos;
    int end_pos;
};

int main(int argc, char *argv[]) {
    if(argc < 3) {
        std::cout << "usage: " << argv[0] << " input.wav wet-factor" << std::endl;
        return 0;
    }

    const float mix = std::atof(argv[2]);
    const std::string srcPath = argv[1];

    if(mix > 1.0 || mix < 0.0) {
        std::cout << "mix factor must be between 0 and 1" << std::endl;
        return 0;
    }

    auto comb1 = std::unique_ptr<CombFilter>(new CombFilter(0.742, 4799));
    auto comb2 = std::unique_ptr<CombFilter>(new CombFilter(0.733, 4999));
    auto comb3 = std::unique_ptr<CombFilter>(new CombFilter(0.715, 5399));
    auto comb4 = std::unique_ptr<CombFilter>(new CombFilter(0.697, 5801));
    auto ap1 = std::unique_ptr<APFilter>(new APFilter(0.7, 347));
    auto ap2 = std::unique_ptr<APFilter>(new APFilter(0.7, 113));

    auto src = std::unique_ptr<wf::WavFile>(new wf::WavFile(srcPath));
    std::vector<int16_t> buffer;
    buffer.resize(src->GetSamplesPerChannel());

    // process 4 parallel comb filters
    const float inputGain = 0.2;
    for(int i=0; i<src->GetSamplesPerChannel(); ++i) {
        int16_t current_sample = src->data[0][i] * inputGain;
        buffer[i] = comb1->process(current_sample) +
                    comb2->process(current_sample) +
                    comb3->process(current_sample) +
                    comb4->process(current_sample);
    }

    // process all-pass 1
    for(int i=0; i<src->GetSamplesPerChannel(); ++i) {
        int16_t current_sample = buffer[i];
        buffer[i] = ap1->process(current_sample);
    }

    // process all-pass 2
    for(int i=0; i<src->GetSamplesPerChannel(); ++i) {
        int16_t current_sample = buffer[i];
        buffer[i] = ap2->process(current_sample);
    }

    // mix dry and wet signals
    for(int i=0; i<src->GetSamplesPerChannel(); ++i) {
        buffer[i] = (mix * buffer[i]) + ((1-mix) * src->data[0][i]);
    }

    auto dst = std::unique_ptr<wf::WavFile>(new wf::WavFile());
    dst->save(buffer, src->GetSampleRate(), "output.wav");

    std::cout << "done!" << std::endl;
    return 0;
}
