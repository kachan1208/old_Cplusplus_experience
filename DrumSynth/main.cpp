#include "DrumSynth.h"
#include "Settings.h"
#include "Wav.h"

#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#endif

int main()
{
#ifndef WIN32
    chdir("/home/kachan/sandbox/DrumSynth/");
#endif
    DrumSynth music;
    music.read("input.ds");

    short *buf = music.data();
    int len = 2 * music.length();

    WavWriter wr;
    wr.open("output.wav", WaveFormat(1, 44100, 16));
    wr.write(buf, len);
    wr.close();
}
