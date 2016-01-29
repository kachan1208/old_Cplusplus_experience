#include "AudioPlayer.h"

#include <string.h>
#include <portaudio.h>
#include <string.h>


AudioPlayer::AudioPlayer()
    : mStream(NULL)
{
    Pa_Initialize();
}

AudioPlayer::~AudioPlayer()
{
    close();
    Pa_Terminate();
}


int AudioPlayer::open(OnAudioPlayerListener* listener, int rate, int channels, 
    int format, size_t frame)
{
    PaError err = Pa_OpenDefaultStream(&mStream, 0, channels, format, rate, 
        frame, &onPlayerCallback, (void*)listener);

    return err;
}

void AudioPlayer::close()
{
    if (mStream)
    {
        if (Pa_IsStreamActive(mStream))
            stop();
        Pa_CloseStream(mStream);
        mStream = NULL;
    }
}

int AudioPlayer::start()
{
    return Pa_StartStream(mStream);
}

void AudioPlayer::stop()
{
    if (mStream)
        Pa_StopStream(mStream);
}


int AudioPlayer::onPlayerCallback(const void* input, void* output,
    unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags, void* userData)
{
    //statusFlags;
    //timeInfo;
    //frameCount;
    //input;
    OnAudioPlayerListener* listener = (OnAudioPlayerListener*)userData;
    if (listener->onPlayerCallback(output, frameCount) > 0)
        return paContinue;

    return paComplete;
}
