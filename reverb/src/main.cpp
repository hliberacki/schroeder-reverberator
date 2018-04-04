#include <iostream>
#include <vector>
#include "WavFile.h"

class APFilter {
public:
    APFilter(float g, int k) {
        m_gain = g;
        m_buffer.resize(k);
        begin_pos = 0;
        end_pos = k-1;
        for(auto it=m_buffer.begin(); it!=m_buffer.end(); ++it) (*it) = 0.0;
    }
    ~APFilter() {
        std::cout << "ap dtor" << std::endl;
    }
    int16_t process(int16_t input) {
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
        m_buffer.resize(k);
        begin_pos = 0;
        end_pos = k-1;
        for(auto it=m_buffer.begin(); it!=m_buffer.end(); ++it) (*it) = 0.0;
    }
    ~CombFilter() {
        std::cout << "comb dtor" << std::endl;
    }
    int16_t process(int16_t input) {
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
    auto src = new wf::WavFile("../../misc/female.wav");

    std::vector<int16_t> buffer;
    buffer.resize(src->GetSamplesPerChannel());

    auto comb1 = new CombFilter(0.742, 4799);
    auto comb2 = new CombFilter(0.733, 4999);
    auto comb3 = new CombFilter(0.715, 5399);
    auto comb4 = new CombFilter(0.697, 5801);

    for(int i=0; i<src->GetSamplesPerChannel(); ++i) {
        int16_t current_sample = src->data[0][i];

        buffer[i] = 0.25 * comb1->process(current_sample) +
                    0.25 * comb2->process(current_sample) +
                    0.25 * comb3->process(current_sample) +
                    0.25 * comb4->process(current_sample);


        /*
        buffer[i] = comb1->process(current_sample) +
                    comb2->process(current_sample) +
                    comb3->process(current_sample) +
                    comb4->process(current_sample);
                    */

    }

    auto ap1 = new APFilter(0.7, 347);
    for(int i=0; i<src->GetSamplesPerChannel(); ++i) {
        int16_t current_sample = buffer[i];
        buffer[i] = ap1->process(current_sample);
    }

    auto ap2 = new APFilter(0.7, 113);
    for(int i=0; i<src->GetSamplesPerChannel(); ++i) {
        int16_t current_sample = buffer[i];
        buffer[i] = ap2->process(current_sample);
    }

    for(int i=0; i<src->GetSamplesPerChannel(); ++i) {
        int16_t current_sample = buffer[i];
        buffer[i] = (0.8 * current_sample) + (0.2 * src->data[0][i]);
    }

    auto dst = std::unique_ptr<wf::WavFile>(new wf::WavFile());
    dst->save(buffer, src->GetSampleRate(), "output.wav");

    return 0;
}
