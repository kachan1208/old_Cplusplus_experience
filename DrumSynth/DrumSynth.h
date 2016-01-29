#include "Settings.h"
#include <map>

class DrumSynth
{
public:
//Constructor/destructor
    DrumSynth();
    ~DrumSynth();

//implementation
    bool read(const char* file);
    short* data();
    int length();

//Members

private:
//implementation
    void getEnv(int env, const char* key, Settings& ini);
    void UpdateEnv(int e, long t);

    int LongestEnv(void);
    float LoudestEnv(void);

    float waveform(float ph, int form);

//Members
    int   busy;
    float envpts[8][3][32];    //envelope/time-level/point
    float envData[8][6];       //envelope running status
    int   chkOn[8], sliLev[8]; //section on/off and level
    float timestretch;         //overall time scaling
    short DD[1200], clippoint;
    float DF[1200];
    float phi[1200];

    short *buf; //for waveform generation in memory buffer
    long  wavewords, wavemode, Length;
    float mem_t, mem_o, mem_n, mem_b, mem_tune, mem_time;
};
