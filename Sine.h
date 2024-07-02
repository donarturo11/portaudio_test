#ifndef SINE_H
#define SINE_H
#include <cmath>
class Sine {
private:
    int _frame;
    int _totalFrames;
    float _samplerate;
    float _freq;
    float _amplitude;
public:
    Sine() : _frame(0) {}
    ~Sine(){}
    void setup(float freq, float samplerate, float amplitude = 1.0f) {
        _samplerate = samplerate;
        _amplitude = amplitude;
        _freq = freq;
        _totalFrames = (int)(_samplerate/_freq);
        _frame = 0;
    }
    float tick() {
        float t=(float)_frame/(float)_totalFrames;
        float value = _amplitude * sin(2*M_PI * t);
        _frame = (_frame + 1) % _totalFrames;
        return value;
    }
    void getSamples(void *output, int maxsize) {
        float *data = (float*) output;
        for (int i=0; i < maxsize; i++) {
            data[i] = tick();
        }
    }
    
};
#endif
