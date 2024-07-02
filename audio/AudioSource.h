#ifndef AUDIO_SOURCE_H
#define AUDIO_SOURCE_H
class AudioSource
{
public:
    virtual float tick() = 0;
    virtual void setRate(unsigned int rate) = 0;
    AudioSource* audioSource(){return (AudioSource*) this; }
};

#endif // AUDIO_SOURCE_H