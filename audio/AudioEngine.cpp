#include "audio/AudioEngine.h"
#include "audio/Buffer.h"
#include <iostream>

AudioEngine::AudioEngine(int nChannels, int nFrames, int nPeriods, int flags, int samplerate)
: _nChannels(nChannels)
, _nFrames(nFrames)
, _nPeriods(nPeriods)
, _samplerate(samplerate)
, _stream_flags(flags)
, _output(nullptr)
, _input(nullptr)
, _running(false)
, _sys(nullptr)
{
    std::cout << "Portaudio instance inside AudioEngine c-tor: ";
    _sys = &(portaudio::System::instance());
    std::cout << (void*) _sys << std::endl;
    initBuffers();    
}

AudioEngine::~AudioEngine()
{
    std::cout << "Terminating audio engine" << std::endl;
    if (_sys)
        _sys->terminate();
}

void AudioEngine::initBuffers()
{
    if (_stream_flags & STREAM_OUTPUT)
        setOutputBuffer();
    if (_stream_flags & STREAM_INPUT)
        setInputBuffer();
}

void AudioEngine::startStream()
{
    
    std::cout << "Starting stream" << std::endl;
    portaudio::StreamParameters streamParams = portaudio::StreamParameters(
                         _inParams,
                         _outParams,
                         _samplerate,
                         _nFrames,
                         paClipOff);
    
    try {
        _stream.open(streamParams, *this, &AudioEngine::handlePaStream);
        _stream.start();
        _running = true;
    }
    catch(...) {
        _running = false;
    }
    
}

void AudioEngine::setupDefaultStreamParameters()
{
    if (!_sys) return;
    std::cout << "Setup stream" << std::endl;
    if (_stream_flags & STREAM_OUTPUT) {
        _outParams = portaudio::DirectionSpecificStreamParameters(
                         _sys->defaultOutputDevice(),
                         _nChannels,
                         portaudio::FLOAT32,
                         false,
                         _sys->defaultOutputDevice().defaultLowOutputLatency(),
                         NULL);
    } else {
        _outParams = portaudio::DirectionSpecificStreamParameters::null();
    }
    if (_stream_flags & STREAM_INPUT) {
        _inParams  = portaudio::DirectionSpecificStreamParameters(
                         _sys->defaultInputDevice(),
                         _nChannels,
                         portaudio::FLOAT32,
                         false,
                         _sys->defaultInputDevice().defaultLowInputLatency(),
                         NULL);
     } else {
         _inParams = portaudio::DirectionSpecificStreamParameters::null();
     }
}

void AudioEngine::stopStream()
{
    std::cout << "Stopping stream" << std::endl;
    _running = false;
    _stream.stop();
    _stream.close();
}

void AudioEngine::readBuffer(void* data,unsigned int nFrames)
{
    if (!_output) return;
    float** out = (float**) data;
    for (unsigned int frame=0; frame<nFrames; frame++){
         if(_output->empty()) break;
         auto val = _output->get();
         for (int channel = 0; channel < val.size(); channel++) {
             out[channel][frame]=val[channel];
         }
    }
}

void AudioEngine::fillBuffer(const void* data,unsigned int nFrames)
{
    if (!_input) return;
    const float** in = (const float**) data;
    for (unsigned int frame=0; frame<nFrames; frame++){
        std::vector<float> v(_nChannels);
        if(_input->full()) break;
        for (int channel = 0; channel<_nChannels; channel++) {
            v[channel]=in[channel][frame];     
        }
        _input->put(v);
    }
}

int AudioEngine::handlePaStream(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags)
{
    if (_stream_flags & STREAM_OUTPUT)
        readBuffer(outputBuffer, framesPerBuffer);
    
    if (_stream_flags & STREAM_INPUT)
        fillBuffer(inputBuffer, framesPerBuffer);
    
    
    return paContinue;
}

