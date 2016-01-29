// PortAudioPlayer.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include "Wav.h"
#include "AudioPlayer.h"
//#include <Windows.h>
#include <vector>
#include <sndfile.h>

int _tmain(int argc, _TCHAR* argv[])
{
    struct WavPlayer : public OnAudioPlayerListener
    {
        std::vector<char> mBuf;
        int mPos;

        WavPlayer()
        {
#if 0
            WavReader rd;
            rd.open("loop2.wav");
            WaveFormat wf = rd.format();
            int len = rd.length() * wf.wBitsPerSample / 8;
            mBuf.resize(len);
            rd.extract(&mBuf[0], len);
#else
            SndfileHandle h("loop2.wav", SFM_READ, SF_FORMAT_WAV | SF_FORMAT_PCM_16, 1, 44100);
            sf_count_t len = h.frames() * sizeof(short);
            mBuf.resize(len);
            h.readRaw(&mBuf[0], len);
#endif
            mPos = 0;
        }

        int onPlayerCallback(void* buf, unsigned long len)
        {
            len *= 2;
            int left = mBuf.size() - mPos;
            int sz = left > len ? len : left;
            memcpy(buf, &mBuf[mPos], sz);
            mPos += sz;
            if (mPos >= mBuf.size())
                mPos = 0;
            return sz;
        }
    };
    WavPlayer wavPlayer;

    AudioPlayer player;
    player.open(&wavPlayer, 44100, 1, paInt16, 1024);
    player.start();

    HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
    DWORD fdwMode = ENABLE_WINDOW_INPUT;
    SetConsoleMode(input, fdwMode);
    while (true)
    {
        INPUT_RECORD buf[1];
        DWORD cNumRead = 0;
        if (!ReadConsoleInput(input, buf, 1, &cNumRead))
            continue;
        if (buf[0].EventType == KEY_EVENT && buf[0].Event.KeyEvent.bKeyDown)// buf[0].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)
            break;
    }

    return 0;
}
