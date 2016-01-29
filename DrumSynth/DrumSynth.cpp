#include "DrumSynth.h"
#include "Settings.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Fs      44100
#define TwoPi   6.2831853f
#define MAX     0
#define ENV     1
#define PNT     2
#define dENV    3
#define NEXTT   4



DrumSynth::DrumSynth()
{
    busy = 0;
    buf = NULL;
    wavemode = 0;
    mem_t = 1.0f;
    mem_o = 1.0f;
    mem_n = 1.0f;
    mem_b = 1.0f;
    mem_tune = 1.0f;
    mem_time = 1.0f;
}

DrumSynth::~DrumSynth()
{
}



bool DrumSynth::read(const char * dsfile)
{
    std::string ver;
    char percent[16];

    //generation
    long  progress=0, tpos=0, tplus, totmp, t, i, j;
    float x[3] = {0.f, 0.f, 0.f};
    float MasterTune, randmax, randmax2;
    int   MainFilter, HighPass;

    long  NON, NT, TON, DiON, TDroop=0, DStep;
    float a, b=0.f, c=0.f, d=0.f, g, TT=0.f, TL, NL, F1, F2;
    float TphiStart=0.f, Tphi, TDroopRate, ddF, DAtten, DGain;

    long  BON, BON2, BFStep, BFStep2, botmp;
    float BdF=0.f, BdF2=0.f, BPhi, BPhi2, BF, BF2, BQ, BQ2, BL, BL2;

    long  OON, OF1Sync=0, OF2Sync=0, OMode, OW1, OW2;
    float Ophi1, Ophi2, OF1, OF2, OL, Ot, OBal1, OBal2, ODrive;
    float Ocf1, Ocf2, OcF, OcQ, OcA, Oc[6][2];  //overtone cymbal mode
    float Oc0=0.0f, Oc1=0.0f, Oc2=0.0f;

    float MFfb, MFtmp, MFres, MFin=0.f, MFout=0.f;
    float DownAve;
    long  DownStart, DownEnd, jj;

    if (wavemode==0) //semi-real-time adjustments if working in memory!!
    {
      mem_t = 1.0f;
      mem_o = 1.0f;
      mem_n = 1.0f;
      mem_b = 1.0f;
      mem_tune = 0.0f;
      mem_time = 1.0f;
    }

    Settings ds;
    ds.read(dsfile);
    ver = ds.getString("General.Version", "");
    if (ver.compare(0, 9, "DrumSynth") != 0)
    {
        busy=0;
        return 2;
    } //input fail

    if (ver[11] != '1' && ver[11] != '2')
    {
        busy=0;
        return 1;
    } //version fail

    timestretch = .01f * mem_time * ds.getFloat("General.Stretch", 100.0);

    if (timestretch < 0.2f)
        timestretch = 0.2f;

    if (timestretch > 10.f)
        timestretch = 10.f;

    DGain = 1.0f; //leave this here!
    DGain = (float)pow(10.0, 0.05 * ds.getFloat("General.Level", 0));

    MasterTune = ds.getFloat("General.Tuning", 0.0);
    MasterTune = (float)pow(1.0594631f, MasterTune + mem_tune);
    MainFilter = 2 * ds.getInt("General.Filter", 0);

    MFres = 0.0101f * ds.getFloat("General.Resonance", 0.0);
    MFres = (float)pow(MFres, 0.5f);

    HighPass = ds.getInt("General.HighPass", 0);
    getEnv(7, "General.FilterEnv", ds);

    //read noise parameters
    chkOn[1] = ds.getInt("Noise.On", 0);
    sliLev[1] = ds.getInt("Noise.Level", 0);
    NT = ds.getInt("Noise.Slope", 0);

    getEnv(2, "Noise.Envelope", ds);

    NON = chkOn[1];
    NL = (float)(sliLev[1] * sliLev[1]) * mem_n;

    if (NT<0)
    {
        a = 1.f + (NT / 105.f);
        d = -NT / 105.f;
        g = (1.f + 0.0005f * NT * NT) * NL;
    }
    else
    {
        a = 1.f;
        b = -NT / 50.f;
        c = (float)fabs((float)NT) / 100.f;
        g = NL;
    }

    srand(1); //fixed random sequence

    chkOn[0] = ds.getInt("Tone.On", 0);
    TON = chkOn[0];

    sliLev[0] = ds.getInt("Tone.Level", 128);
    TL = (float)(sliLev[0] * sliLev[0]) * mem_t;

    getEnv(1, "Tone.Envelope", ds);

    F1 = MasterTune * TwoPi * ds.getFloat("Tone.F1", 200.0) / Fs;
    if (fabs(F1) < 0.001f)
        F1 = 0.001f; //to prevent overtone ratio div0

    F2 = MasterTune * TwoPi * ds.getFloat("Tone.F2", 120.0) / Fs;
    TDroopRate = ds.getFloat("Tone.Droop", 0.f);

    if (TDroopRate > 0.f)
    {
        TDroopRate = (float)pow(10.0f, (TDroopRate - 20.0f) / 30.0f);
        TDroopRate = TDroopRate * -4.f / envData[1][MAX];
        TDroop = 1;
        F2 = F1 + ((F2-F1) / (1.f - (float)exp(TDroopRate * envData[1][MAX])));
        ddF = F1 - F2;
    }
    else
        ddF = F2-F1;

    Tphi = ds.getFloat("Tone.Phase", 90.f) / 57.29578f; //degrees>radians

    //read overtone parameters

    chkOn[2] = ds.getInt("Overtones.On", 0);
    OON = chkOn[2];

    sliLev[2] = ds.getInt("Overtones.Level", 128);
    OL = (float)(sliLev[2] * sliLev[2]) * mem_o;

    getEnv(3, "Overtones.Envelope1", ds);
    getEnv(4, "Overtones.Envelope2", ds);
    OMode = ds.getInt("Overtones.Method", 2);

    OF1 = MasterTune * TwoPi * ds.getFloat("Overtones.F1", 200.0) / Fs;
    OF2 = MasterTune * TwoPi * ds.getFloat("Overtones.F2", 120.0) / Fs;

    OW1 = ds.getInt("Overtones.Wave1", 0);
    OW2 = ds.getInt("Overtones.Wave2", 0);

    OBal2 = (float)ds.getInt("Overtones.Param", 50);
    ODrive = (float)pow(OBal2, 3.0f) / (float)pow(50.0f, 3.0f);
    OBal2 *= 0.01f;
    OBal1 = 1.f - OBal2;

    Ophi1 = Tphi;
    Ophi2 = Tphi;

    if (MainFilter == 0)
        MainFilter = ds.getInt("Overtones.Filter", 0);

    if ((ds.getInt("Overtones.Track1", 0) == 1) && (TON == 1))
    {
        OF1Sync = 1;
        OF1 = OF1 / F1;
    }

    if ((ds.getInt("Overtones.Track2", 0) == 1) && (TON == 1))
    {
        OF2Sync = 1;
        OF2 = OF2 / F1;
    }

    OcA = 0.28f + OBal1 * OBal1;  //overtone cymbal mode
    OcQ = OcA * OcA;
    OcF = (1.8f - 0.7f * OcQ) * 0.92f; //multiply by env 2
    OcA *= 1.0f + 4.0f * OBal1;  //level is a compromise!
    Ocf1 = TwoPi / OF1;
    Ocf2 = TwoPi / OF2;
    for (i = 0; i < 6; i++)
        Oc[i][0] = Oc[i][1] = Ocf1 + (Ocf2 - Ocf1) * 0.2f * (float)i;

    //read noise band parameters

    chkOn[3] = ds.getInt("NoiseBand.On", 0);
    BON = chkOn[3];

    sliLev[3] = ds.getInt("NoiseBand.Level", 128);
    BL = (float)(sliLev[3] * sliLev[3]) * mem_b;

    BF = MasterTune * TwoPi * ds.getFloat("NoiseBand.F", 1000.0) / Fs;
    BPhi = TwoPi / 8.f;

    getEnv(5, "NoiseBand.Envelope", ds);

    BFStep = ds.getInt("NoiseBand.dF",50);
    BQ = (float)BFStep;
    BQ = BQ * BQ / (10000.f-6600.f*((float)sqrt(BF)-0.19f));
    BFStep = 1 + (int)((40.f - (BFStep / 2.5f)) / (BQ + 1.f + (1.f * BF)));

    chkOn[4] = ds.getInt("NoiseBand2.On", 0);
    BON2 = chkOn[4];

    sliLev[4] = ds.getInt("NoiseBand2.Level", 128);
    BL2 = (float)(sliLev[4] * sliLev[4]) * mem_b;
    BF2 = MasterTune * TwoPi * ds.getFloat("Distortion.F", 1000.0) / Fs;
    BPhi2 = TwoPi / 8.f;

    getEnv(6, "NoiseBand2.Envelope", ds);

    BFStep2 = ds.getInt("NoiseBand2.dF", 50);
    BQ2 = (float)BFStep2;
    BQ2 = BQ2 * BQ2 / (10000.f-6600.f*((float)sqrt(BF2)-0.19f));
    BFStep2 = 1 + (int)((40 - (BFStep2 / 2.5)) / (BQ2 + 1 + (1 * BF2)));

    //read distortion parameters

    chkOn[5] = ds.getInt("Distortion.On",0);
    DiON = chkOn[5];

    DStep = 1 + ds.getInt("Distortion.Rate",0);
    if (DStep == 7)
        DStep = 20;

    if (DStep == 6)
        DStep = 10;

    if (DStep == 5)
        DStep = 8;

    clippoint = 32700;
    DAtten = 1.0f;

    if (DiON==1)
    {
        DAtten = DGain * (short)LoudestEnv();
        if (DAtten>32700)
            clippoint = 32700;
        else
            clippoint = (short)DAtten;

      DAtten = (float)pow(2.0, 2.0 * ds.getInt("Distortion.Bits", 0));
      DGain = DAtten * DGain * (float)pow(10.0, 0.05 * ds.getInt("Distortion.Clipping", 0));
    }

    //prepare envelopes
    randmax = 1.f / RAND_MAX;
    randmax2 = 2.f * randmax;

    for (i = 1; i < 8; i++)
    {
        envData[i][NEXTT]=0;
        envData[i][PNT]=0;
    }

    Length = LongestEnv();

    if (buf != NULL)
        free(buf);

    buf = (short *)malloc(2 * Length); //wave memory buffer
    if (buf == NULL)
    {
        busy=0;
        return 3;
    }

    wavewords = 0;

    //generate
    tpos = 0;
    while (tpos < Length)
    {
        tplus = tpos + 1199;

        if (NON == 1) //noise
        {
            for (t = tpos; t <= tplus; t++)
            {
            if (t < envData[2][NEXTT])
                envData[2][ENV] = envData[2][ENV] + envData[2][dENV];
            else
                UpdateEnv(2, t);

            x[2] = x[1];
            x[1] = x[0];
            x[0] = (randmax2 * (float)rand()) - 1.f;
            TT = a * x[0] + b * x[1] + c * x[2] + d * TT;
            DF[t - tpos] = TT * g * envData[2][ENV];
        }

        if (t >= envData[2][MAX])
            NON = 0;
        }

        else
            for (j = 0; j < 1200; j++)
                DF[j] = 0.f;

        if (TON == 1) //tone
        {
            TphiStart = Tphi;
            if (TDroop == 1)
            {
            for (t = tpos; t <= tplus; t++)
                phi[t - tpos] = F2 + (ddF * (float)exp(t * TDroopRate));
            }
            else
            {
                for (t = tpos; t <= tplus; t++)
                    phi[t - tpos] = F1 + (t / envData[1][MAX]) * ddF;
            }
            for (t = tpos; t <= tplus; t++)
            {
                totmp = t - tpos;
                if (t < envData[1][NEXTT])
                    envData[1][ENV] = envData[1][ENV] + envData[1][dENV];
                else
                    UpdateEnv(1, t);

                Tphi = Tphi + phi[totmp];
                DF[totmp] += TL * envData[1][ENV] * (float)sin(fmod(Tphi,TwoPi));//overflow?
            }
            if (t >= envData[1][MAX])
                TON = 0;
        }
        else
            for (j = 0; j < 1200; j++)
                phi[j] = F2; //for overtone sync

        if (BON == 1) //noise band 1
        {
            for (t = tpos; t <= tplus; t++)
            {
            if (t < envData[5][NEXTT])
                envData[5][ENV] = envData[5][ENV] + envData[5][dENV];
            else
                UpdateEnv(5, t);

            if ((t % BFStep) == 0)
                BdF = randmax * (float)rand() - 0.5f;

            BPhi = BPhi + BF + BQ * BdF;
            botmp = t - tpos;
            DF[botmp] = DF[botmp] + (float)cos(fmod(BPhi,TwoPi)) * envData[5][ENV] * BL;
            }

            if (t >= envData[5][MAX])
                BON = 0;
        }

        if (BON2 == 1) //noise band 2
        {
            for (t = tpos; t <= tplus; t++)
            {
                if (t < envData[6][NEXTT])
                    envData[6][ENV] = envData[6][ENV] + envData[6][dENV];
                else
                    UpdateEnv(6, t);

                if ((t % BFStep2) == 0)
                    BdF2 = randmax * (float)rand() - 0.5f;

                BPhi2 = BPhi2 + BF2 + BQ2 * BdF2;
                botmp = t - tpos;
                DF[botmp] = DF[botmp] + (float)cos(fmod(BPhi2,TwoPi)) * envData[6][ENV] * BL2;
            }

            if (t >= envData[6][MAX])
                BON2 = 0;
        }

        for (t = tpos; t <= tplus; t++)
        {
            if (OON == 1) //overtones
            {
                if (t < envData[3][NEXTT])
                    envData[3][ENV] = envData[3][ENV] + envData[3][dENV];
                else
                {
                    if (t >= envData[3][MAX]) //wait for OT2
                    {
                        envData[3][ENV] = 0;
                        envData[3][dENV] = 0;
                        envData[3][NEXTT] = 999999;
                    }
                    else
                        UpdateEnv(3, t);
                }
          //
                if (t < envData[4][NEXTT])
                    envData[4][ENV] = envData[4][ENV] + envData[4][dENV];
                else
                {
                    if (t >= envData[4][MAX]) //wait for OT1
                    {
                        envData[4][ENV] = 0;
                        envData[4][dENV] = 0;
                        envData[4][NEXTT] = 999999;
                    }
                    else
                        UpdateEnv(4, t);
                }
          //
            TphiStart = TphiStart + phi[t - tpos];
            if (OF1Sync == 1)
                Ophi1 = TphiStart * OF1;
            else
                Ophi1 = Ophi1 + OF1;

            if (OF2Sync == 1)
                Ophi2 = TphiStart * OF2;
            else
                Ophi2 = Ophi2 + OF2;

            Ot = 0.0f;
            switch (OMode)
            {
                case 0: //add
                    Ot = OBal1 * envData[3][ENV] * waveform(Ophi1, OW1);
                    Ot = OL * (Ot + OBal2 * envData[4][ENV] * waveform(Ophi2, OW2));
                    break;

                case 1: //FM
                    Ot = ODrive * envData[4][ENV] * waveform(Ophi2, OW2);
                    Ot = OL * envData[3][ENV] * waveform(Ophi1 + Ot, OW1);
                    break;

                case 2: //RM
                    Ot = (1 - ODrive / 8) + (((ODrive / 8) * envData[4][ENV]) * waveform(Ophi2, OW2));
                    Ot = OL * envData[3][ENV] * waveform(Ophi1, OW1) * Ot;
                    break;

                case 3: //808 Cymbal
                    for (j = 0; j < 6; j++)
                    {
                        Oc[j][0] += 1.0f;
                        if (Oc[j][0] > Oc[j][1])
                        {
                            Oc[j][0] -= Oc[j][1];
                            Ot = OL * envData[3][ENV];
                        }
                    }

                    Ocf1 = envData[4][ENV] * OcF;  //filter freq
                    Oc0 += Ocf1 * Oc1;
                    Oc1 += Ocf1 * (Ot + Oc2 - OcQ * Oc1 - Oc0);  //bpf
                    Oc2 = Ot;
                    Ot = Oc1;
                    break;
                }
            }

        if (MainFilter == 1) //filter overtones
        {
            if (t < envData[7][NEXTT])
                envData[7][ENV] = envData[7][ENV] + envData[7][dENV];
            else
                UpdateEnv(7, t);

            MFtmp = envData[7][ENV];
            if (MFtmp >0.2f)
                MFfb = 1.001f - (float)pow(10.0f, MFtmp - 1);
            else
                MFfb = 0.999f - 0.7824f * MFtmp;

            MFtmp = Ot + MFres * (1.f + (1.f/MFfb)) * (MFin - MFout);
            MFin = MFfb * (MFin - MFtmp) + MFtmp;
            MFout = MFfb * (MFout - MFin) + MFin;
            DF[t - tpos] = DF[t - tpos] + (MFout - (HighPass * Ot));
        }

        else if (MainFilter == 2) //filter all
        {
            if (t < envData[7][NEXTT])
                envData[7][ENV] = envData[7][ENV] + envData[7][dENV];
            else
                UpdateEnv(7, t);

            MFtmp = envData[7][ENV];
            if (MFtmp > 0.2f)
                MFfb = 1.001f - (float)pow(10.0f, MFtmp - 1);
            else
                MFfb = 0.999f - 0.7824f * MFtmp;

            MFtmp = DF[t - tpos] + Ot + MFres * (1.f + (1.f/MFfb)) * (MFin - MFout);
            MFin = MFfb * (MFin - MFtmp) + MFtmp;
            MFout = MFfb * (MFout - MFin) + MFin;
            DF[t - tpos] = MFout - (HighPass * (DF[t - tpos] + Ot));
        }
        else
            DF[t - tpos] = DF[t - tpos] + Ot; //no filter
      }

        if (DiON == 1) //bit resolution
        {
            for (j = 0; j < 1200; j++)
                DF[j] = DGain * (int)(DF[j] / DAtten);

            for (j = 0; j < 1200; j += DStep) //downsampling
            {
                DownAve = 0;
                DownStart = j;
                DownEnd = j + DStep - 1;
                for (jj = DownStart; jj <= DownEnd; jj++)
                    DownAve = DownAve + DF[jj];

                DownAve = DownAve / DStep;
                for (jj = DownStart; jj <= DownEnd; jj++)
                    DF[jj] = DownAve;
            }
        }

        else
            for (j = 0; j < 1200; j++)
                DF[j] *= DGain;

        for (j = 0; j < 1200; j++) //clipping + output
        {
            if (DF[j] > clippoint)
                buf[wavewords++] = clippoint;

            else
            {
                if (DF[j] < -clippoint)
                    buf[wavewords++] = -clippoint;
                else
                    buf[wavewords++] = (short)DF[j];
            }
        }

        if (((100*tpos) / Length) > (progress + 4))
        {
            progress = (100*tpos) / Length;
            sprintf(percent, "%i%%", progress);
        }
        tpos = tpos + 1200;
    }
    return 0;
}

short* DrumSynth::data()
{
    return buf;
}

int DrumSynth::length()
{
    return Length;
}

void DrumSynth::getEnv(int env, const char* key, Settings& ini)
{
    char en[257];
    char s[8];
    int i=0, o=0, ep=0;
    strcpy(en, ini.getString(key, "0,0 100,0").c_str());
    en[256] = 0;

    while (en[i]!=0)
    {
        if (en[i] == ',')
        {
            if (sscanf(s, "%f", &envpts[env][0][ep])==0)
                envpts[env][0][ep] = 0.f;

            o=0;
        }
        else if (en[i] == ' ')
        {
            if (sscanf(s, "%f", &envpts[env][1][ep])==0)
                envpts[env][1][ep] = 0.f;

            o=0; ep++;
        }
        else
        {
            s[o]=en[i];
            o++;
            s[o]=0;
        }

        i++;
    }

    if (sscanf(s, "%f", &envpts[env][1][ep])==0)
        envpts[env][1][ep] = 0.f;

    envpts[env][0][ep + 1] = -1;
    envData[env][MAX] = envpts[env][0][ep];
}

int DrumSynth::LongestEnv(void)
{
    long e, eon, p;
    float l = 0.f;

    for (e = 1; e < 7; e++) //3
    {
        eon = e - 1;
        if (eon > 2)
            eon = eon - 1;

        p = 0;
        while (envpts[e][0][p + 1] >= 0.f)
            p++;

        envData[e][MAX] = envpts[e][0][p] * timestretch;
        if (chkOn[eon] == 1)
            if (envData[e][MAX] > l)
                l = envData[e][MAX];
  }
  //l *= timestretch;
  return 2400 + (1200 * (int)(l / 1200));
}

float DrumSynth::LoudestEnv(void)
{
    float loudest=0.f;
    int i=0;

    while (i < 5) //2
    {
        if (chkOn[i] == 1)
            if (sliLev[i] > loudest)
                loudest = (float)sliLev[i];

        i++;
    }

    return (loudest * loudest);
}

void DrumSynth::UpdateEnv(int e, long t)
{
    float endEnv, dT;
                                                             //0.2's added
    envData[e][NEXTT] = envpts[e][0][(long)(envData[e][PNT] + 1.f)] * timestretch; //get next point

    if (envData[e][NEXTT] < 0)
        envData[e][NEXTT] = 442000 * timestretch; //if end point, hold

    envData[e][ENV] = envpts[e][1][(long)(envData[e][PNT] + 0.f)] * 0.01f; //this level
    endEnv = envpts[e][1][(long)(envData[e][PNT] + 1.f)] * 0.01f;          //next level
    dT = envData[e][NEXTT] - (float)t;

    if (dT < 1.0)
        dT = 1.0;

    envData[e][dENV] = (endEnv - envData[e][ENV]) / dT;
    envData[e][PNT] = envData[e][PNT] + 1.0f;
}

float DrumSynth::waveform(float ph, int form)
{
    float w;

    switch (form)
    {
        case 0:
            w = (float)sin(fmod(ph,TwoPi));
            break; //sine

        case 1:
            w = (float)fabs(2.0f * (float)sin(fmod(0.5f*ph, TwoPi))) - 1.f;
            break; //sine^2

        case 2:
            while (ph < TwoPi)
                ph += TwoPi;

            w = 0.6366197f * (float)fmod(ph,TwoPi) - 1.f;//tri
            if (w > 1.f)
                w = 2.f-w;
            break;

        case 3:
            w = ph - TwoPi * (float)(int)(ph / TwoPi);//saw
            w = (0.3183098f * w) - 1.f;
            break;

        default:
            w = (sin(fmod(ph,TwoPi))>0.0)? 1.f: -1.f;
            break; //square
    }

    return w;
}
