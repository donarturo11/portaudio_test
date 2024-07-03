#include "audio/AudioEngine.h"
#include "audio/Buffer.h"
#ifdef USE_PORTAUDIOCPP
#include <algorithm>
#endif
#ifdef DEBUG
#include <iostream>
#endif

AudioEngine::AudioEngine(int nChannels, int nFrames, int nPeriods, int flags, int samplerate)
: _interleaved(true)
, _nChannels(nChannels)
, _nFrames(nFrames)
, _nPeriods(nPeriods)
, _samplerate(samplerate)
, _stream_flags(flags)
//, _output(nullptr)
//, _input(nullptr)
, _running(false)
#ifdef USE_PORTAUDIOCPP
, _sampleformat(portaudio::FLOAT32)
, _sys(nullptr)
#else
, _sampleformat(paFloat32)
, _paError(Pa_Initialize())
, _outDev(paNoDevice)
, _inDev(paNoDevice)
, _stream(nullptr)
#endif
{
#ifdef USE_PORTAUDIOCPP
    _sys = &(portaudio::System::instance());
#else
    if (!_interleaved)
        _sampleformat |= paNonInterleaved;
    else
        _sampleformat &= ~paNonInterleaved;
#endif
    initBuffers();
}

AudioEngine::~AudioEngine()
{
#ifdef DEBUG
    std::cout << "Terminating audio engine" << std::endl;
#endif
#ifdef USE_PORTAUDIOCPP
    if (_sys)
        _sys->terminate();
#else
    if (_paError == paNoError)
        Pa_Terminate();
#endif
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
#ifdef USE_PORTAUDIOCPP
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
#else
    PaStreamParameters streamParams;
    PaError err = Pa_OpenStream(
          &_stream,
          ((_stream_flags & STREAM_INPUT) ? &_inParams : nullptr),
          ((_stream_flags & STREAM_OUTPUT) ? &_outParams : nullptr),
          _samplerate,
          _nFrames,
          paClipOff,
          AudioEngine::paStreamCallback,
          this );
    if (!err) {
        err = Pa_StartStream(_stream);
    }
    _running = (!err);
#endif
}

void AudioEngine::setupDefaultStreamParameters()
{
#ifdef USE_PORTAUDIOCPP
    if (!_sys) return;
    if (_stream_flags & STREAM_OUTPUT) {
        _outParams = portaudio::DirectionSpecificStreamParameters(
                         *_outDev,
                         _nChannels,
                         portaudio::FLOAT32,
                         _interleaved,
                         _outDev->defaultLowOutputLatency(),
                         NULL);
    } else {
        _outParams = portaudio::DirectionSpecificStreamParameters::null();
    }
    if (_stream_flags & STREAM_INPUT) {
        _inParams  = portaudio::DirectionSpecificStreamParameters(
                         *_inDev,
                         _nChannels,
                         _sampleformat,
                         _interleaved,
                         _inDev->defaultLowInputLatency(),
                         NULL);
     } else {
         _inParams = portaudio::DirectionSpecificStreamParameters::null();
     }
#else
    if (_stream_flags & STREAM_OUTPUT) {
        _outParams = {
            _outDev,
            _nChannels,
            _sampleformat,
            Pa_GetDeviceInfo( _outDev )->defaultLowOutputLatency,
            nullptr
        };
    }
    if (_stream_flags & STREAM_INPUT) {
        _inParams = {
            _inDev,
            _nChannels,
            _sampleformat,
            Pa_GetDeviceInfo( _inDev )->defaultLowInputLatency,
            nullptr
        };
     }
#endif
}

void AudioEngine::setupHostApi(PaHostApiTypeId hostid)
{
#ifdef USE_PORTAUDIOCPP
    auto apiFound = [hostid](portaudio::HostApi& api){
        return api.typeId()==hostid;
    };
    auto it = std::find_if(_sys->hostApisBegin(), _sys->hostApisEnd(), apiFound);
    if (it != _sys->hostApisEnd()) {
        portaudio::HostApi *api = &(*it);
        _outDev = &(api->defaultOutputDevice());
        _inDev = &(api->defaultInputDevice());
    } else {
        setupDefaultDevice();
    }
    _samplerate = _outDev->defaultSampleRate();
#else
    bool apiFound = false;
    int numDevices = Pa_GetDeviceCount();
    for (int i = 0; i<numDevices; i++) {
        const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
        auto api = Pa_GetHostApiInfo( deviceInfo->hostApi );
        if (api->type == hostid) {
            _inDev = api->defaultInputDevice;
            _outDev = api->defaultOutputDevice;
            apiFound = true;
            break;
        }
    }
    if (!apiFound)
        setupDefaultDevice();
    _samplerate = Pa_GetDeviceInfo( _outDev )->defaultSampleRate;
#endif
}

void AudioEngine::setupDefaultDevice()
{
#ifdef USE_PORTAUDIOCPP
    _outDev = &(_sys->defaultOutputDevice());
    _inDev = &(_sys->defaultInputDevice());
#else
    _outDev = Pa_GetDefaultOutputDevice();
    _inDev = Pa_GetDefaultInputDevice();
#endif
}

void AudioEngine::stopStream()
{
    _running = false;
#ifdef USE_PORTAUDIOCPP
    _stream.stop();
    _stream.close();
#else
    Pa_StopStream(_stream);
    Pa_CloseStream(_stream);
#endif
}

void AudioEngine::readBuffer(void* data,unsigned int nFrames)
{
    if (!_output) return;
    float* out = (float*) data;
    _output->read(out, nFrames*_nChannels);
}

void AudioEngine::fillBuffer(const void* data,unsigned int nFrames)
{
    if (!_input) return;
    const float* in = (const float*) data;
    _input->write(in, nFrames*_nChannels);
}

int AudioEngine::paStreamCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData)
{
    auto self = reinterpret_cast<AudioEngine*>(userData);
    return self->handlePaStream(inputBuffer,
                                outputBuffer,
                                framesPerBuffer,
                                timeInfo,
                                statusFlags);
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

