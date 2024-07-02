#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H
#include "portaudiocpp/PortAudioCpp.hxx"
#include <vector>
#include <memory>
#include "audio/AudioSource.h"
#include "audio/Buffer.h"

#define STREAM_OUTPUT 1
#define STREAM_INPUT 2
#define STREAM_DUPLEX (STREAM_OUTPUT | STREAM_INPUT)
using AudioBuffer = Buffer<std::vector<float>>;
class AudioEngine
{
public:
    AudioEngine(int nChannels = 1, int nFrames = 512, int nPeriods = 1, int flags = STREAM_DUPLEX, int samplerate = 44100);
    ~AudioEngine();
    
    int handlePaStream(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags);
    void initBuffers();
    void readBuffer(void*,unsigned int);
    void fillBuffer(const void*,unsigned int);
    
    AudioBuffer * output() { return _output; }
    AudioBuffer * input() { return _input; }
    unsigned int outputBufferSize() const {
        return _output ? _output->capacity() : 0;
    }
    unsigned int inputBufferSize() const {
        return _input ? _input->capacity() : 0;
    }
    void setupInputOnly() { _stream_flags = STREAM_INPUT; }
    void setupOutputOnly() { _stream_flags = STREAM_OUTPUT; }
    void setupDuplex() { _stream_flags = (STREAM_INPUT | STREAM_OUTPUT); }
    void startStream();
    void setupDefaultStreamParameters();
    void stopStream();
    void setSamplerate(unsigned int r) { _samplerate = r; }
    int samplerate() const { return _samplerate; }
    int periods() const { return _nPeriods; }
    int channels() const { return _nChannels; }
    int frames() const { return _nFrames; }
    void setChannelsNumber(unsigned int n) {
        _nChannels = n;
        resizeBuffer();
        //_outParams.setNumChannels(_nChannels);
        //_inParams.setNumChannels(_nChannels);
        
    }
    void setPeriodsNumber(unsigned int n) {
        _nPeriods = n;
        resizeBuffer();
    }
    
    bool running() const {
        return _running;
    }
    
    void setInputBuffer(AudioBuffer* in = nullptr) {
        if (_input) {
            delete _input;
            _input = nullptr;
        }
        if (in) {
            _input = in;
        }
        else _input = new AudioBuffer(_nFrames*_nChannels*_nPeriods);
        
    }
    
    void setOutputBuffer(AudioBuffer* out = nullptr) {
        if (_output) {
             delete _output;
             _output = nullptr;
        }
        if (out) {
            _output = out;
        }
        else _output = new AudioBuffer(_nFrames*_nChannels*_nPeriods);
    }
    
    portaudio::System* paSystem() const { return _sys; }
    
private:
    void resizeBuffer() {
        _input->resize(_nFrames * _nChannels * _nPeriods);
        _output->resize(_nFrames * _nChannels * _nPeriods);
    }
protected:
    portaudio::AutoSystem autoSys;
    portaudio::System *_sys;
    portaudio::MemFunCallbackStream<AudioEngine> _stream;
    //portaudio::StreamParameters _streamParams;
    portaudio::DirectionSpecificStreamParameters _outParams;
    portaudio::DirectionSpecificStreamParameters _inParams;
    AudioBuffer* _input;
    AudioBuffer* _output;
    int _stream_flags;
    unsigned int _nChannels;
    unsigned int _samplerate;
    unsigned int _nFrames;
    unsigned int _nPeriods;
    bool _running;
    

};

#endif // AUDIOENGINE_H
