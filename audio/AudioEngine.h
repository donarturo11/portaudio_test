#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H
#ifdef USE_PORTAUDIOCPP
#include "portaudiocpp/PortAudioCpp.hxx"
#endif
#include "portaudio.h"
#include <vector>
#include <memory>
#include "audio/Buffer.h"

#define STREAM_OUTPUT 1
#define STREAM_INPUT 2
#define STREAM_DUPLEX (STREAM_OUTPUT | STREAM_INPUT)
using AudioBuffer = Buffer<float>;
class AudioEngine
{
public:
    AudioEngine(int nChannels = 1, int nFrames = 512, int nPeriods = 1, int flags = STREAM_DUPLEX, int samplerate = 44100);
    ~AudioEngine();
    int handlePaStream(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags);
    static int paStreamCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData);
    void initBuffers();
    void readBuffer(void*,unsigned int);
    void fillBuffer(const void*,unsigned int);
    AudioBuffer * output() { return _output.get(); }
    AudioBuffer * input() { return _input.get(); }
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
    void setupHostApi(PaHostApiTypeId hostid);
    void setupDefaultDevice();
    void stopStream();
    void setSamplerate(unsigned int r) { _samplerate = r; }
    int samplerate() const { return _samplerate; }
    int periods() const { return _nPeriods; }
    int channels() const { return _nChannels; }
    int frames() const { return _nFrames; }
    void setChannelsNumber(unsigned int n) {
        _nChannels = n;
        resizeBuffer();
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
            _input.reset();
        }
        if (in) {
            _input = std::unique_ptr<AudioBuffer>(in);
        }
        else _input = std::make_unique<AudioBuffer>(_nFrames*_nChannels*_nPeriods);
    }
    void setOutputBuffer(AudioBuffer* out = nullptr) {
        if (_output) {
             _output.reset();
        }
        if (out) {
            _output = std::unique_ptr<AudioBuffer>(out);
        }
        else _output = std::make_unique<AudioBuffer>(_nFrames*_nChannels*_nPeriods);
    }
private:
    void resizeBuffer() {
        _input->resize(_nFrames * _nChannels * _nPeriods);
        _output->resize(_nFrames * _nChannels * _nPeriods);
    }
protected:
    const bool _interleaved;
#ifdef USE_PORTAUDIOCPP
    portaudio::SampleDataFormat _sampleformat;
    portaudio::AutoSystem autoSys;
    portaudio::System *_sys;
    portaudio::MemFunCallbackStream<AudioEngine> _stream;
    portaudio::DirectionSpecificStreamParameters _outParams;
    portaudio::DirectionSpecificStreamParameters _inParams;
    portaudio::Device *_outDev;
    portaudio::Device *_inDev;
#else
    PaSampleFormat _sampleformat;
    PaStream *_stream;
    PaStreamParameters _outParams;
    PaStreamParameters _inParams;
    PaDeviceIndex _outDev;
    PaDeviceIndex _inDev;
    PaError _paError;
#endif
    std::unique_ptr<AudioBuffer> _input;
    std::unique_ptr<AudioBuffer> _output;
    int _stream_flags;
    int _nChannels;
    unsigned int _samplerate;
    unsigned int _nFrames;
    unsigned int _nPeriods;
    bool _running;
};
#endif // AUDIOENGINE_H
