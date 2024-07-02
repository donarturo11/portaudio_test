#include "AudioEngine.h"
#include "Sine.h"
#include "portaudiocpp/PortAudioCpp.hxx"
#include <iostream>
#include <chrono>
#include <thread>

static bool running = false;
void fillBuffer(AudioBuffer* buf, Sine* s) {
    while (!(buf->full())) {
        float v[2];
        v[0] = s->tick();
        v[1] = v[0];
        buf->write((const float*) v, 2);
    }
    if (buf->full()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if (running) {
        fillBuffer(buf, s);
    }
}

int main(int argc,  char **argv)
{
    AudioEngine audio(2, 4096, 4, STREAM_OUTPUT);
    
    audio.setupDefaultStreamParameters();
    
    float freq;
    Sine s;
    s.setup(440, 44100, 0.9);
    running = true;
    auto buf = audio.output();
    std::thread t(fillBuffer,buf, &s);
    t.detach();
    while (!(buf->full())) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    audio.startStream();
    std::cout << std::endl
              << "Type desired frequency to change in range freq > 0 " << std::endl 
              << "or any key to quit then press ENTER" << std::endl;
    while (running) {
        std::cout << "> ";
        std::cin >> freq;
        s.setup(freq, 44100, 0.9);
        if (freq<=0) {
            audio.stopStream();
            running = false;
        }
    }
    
    return 0;
}
