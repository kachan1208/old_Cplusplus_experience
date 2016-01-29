#pragma once
#include <portaudio.h>
#include <string.h>

struct OnAudioPlayerListener
{
    virtual int onPlayerCallback(void* buf, unsigned long len) = 0;
};

/**
 * AudioPlayer plays PCM data
 */
class AudioPlayer
{
public:
// Constructor/Destructor
    AudioPlayer();
    virtual ~AudioPlayer();

// Implementation
    int  open(OnAudioPlayerListener* listener, int rate, int channels,
        int format, size_t frame);

    void close();

    int  start();
    void stop();

protected:
    static int onPlayerCallback(const void* input, void* output,
        unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags, void* userData);

private:
// Members
    PaStream* mStream;
};

