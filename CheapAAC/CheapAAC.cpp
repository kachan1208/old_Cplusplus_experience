#include <list>
#include <iostream>
#include "CheapAAC.h"


static const int kDINF = 0x64696e66;
static const int kFTYP = 0x66747970;
static const int kHDLR = 0x68646c72;
static const int kMDAT = 0x6d646174;
static const int kMDHD = 0x6d646864;
static const int kMDIA = 0x6d646961;
static const int kMINF = 0x6d696e66;
static const int kMOOV = 0x6d6f6f76;
static const int kMP4A = 0x6d703461;
static const int kMVHD = 0x6d766864;
static const int kSMHD = 0x736d6864;
static const int kSTBL = 0x7374626c;
static const int kSTCO = 0x7374636f;
static const int kSTSC = 0x73747363;
static const int kSTSD = 0x73747364;
static const int kSTSZ = 0x7374737a;
static const int kSTTS = 0x73747473;
static const int kTKHD = 0x746b6864;
static const int kTRAK = 0x7472616b;

const int kRequiredAtoms[14] = {
    kDINF, kHDLR, kMDHD, kMDIA, kMINF, kMOOV, kMVHD, kSMHD, kSTBL, kSTSD, kSTSZ,
    kSTTS, kTKHD, kTRAK
};

const int kSaveDataAtoms[7] = {
    kDINF, kHDLR, kMDHD, kMVHD, kSMHD, kTKHD, kSTSD
};


int toInt (char* buf)
{
   return ((0xff & buf[0]) << 24) |
          ((0xff & buf[1]) << 16) |
          ((0xff & buf[2]) << 8)  |
          ((0xff & buf[3]));
}



CheapAAC::CheapAAC()
{
    mFrameOffsets = NULL;
    mFrameLens = NULL;
    mFrameGains = NULL;
}

CheapAAC::~CheapAAC()
{
    delete[] mFrameOffsets;
    delete[] mFrameLens;
    delete[] mFrameGains;
}



int CheapAAC::readFile(const char* inputFile)
{
    mChannels = 0;
    mSampleRate = 0;
    mBitrate = 0;
    mSamplesPerFrame = 0;
    mNumFrames = 0;
    mMinGain = 255;
    mMaxGain = 0;
    mOffset = 0;
    mMdatOffset = -1;
    mMdatLength = -1;

    std::ifstream stream(inputFile, std::ifstream::binary);
    stream.seekg(0, std::fstream::end);
    int fileSize = stream.tellg();
    if (fileSize < 128)
        return -1;

    stream.seekg(0);
    char header[8];
    stream.read(header, 8);
    if (header[0] != 0 || memcmp(&header[4], "ftyp", 4) != 0)
        return -1;

    stream.seekg(0);
    parseMp4(stream, fileSize);
    if (mMdatOffset < 0 || mMdatLength < 0)
        return -1;

    stream.seekg(mMdatOffset, std::ios_base::cur);
    mOffset = mMdatOffset;
    parseMdat(stream, mMdatLength);
    return 1;
}

int CheapAAC::parseMp4(std::ifstream& stream, int maxLen)
{
    while (maxLen > 8)
    {
        int initialOffset = mOffset;
        char atomHeader[8];

        stream.read(atomHeader, 8);
        int atomLen = toInt((char*)&atomHeader[0]);


        std::cout << "atomType = " << atomHeader[4] << atomHeader[5] <<
                atomHeader[6] <<atomHeader[7] << "  " <<"offset = " <<
                     mOffset <<"  "<<"atomLen = "<<atomLen<<"\n";

        if (atomLen > maxLen)
            atomLen = maxLen;

        int atomType = toInt((char*)&atomHeader[4]);

        Atom &atom = mAtomMap[atomType];
        atom.start = mOffset;
        atom.len = atomLen;
        mOffset += 8;

        if (atomType == kMOOV ||
            atomType == kTRAK ||
            atomType == kMDIA ||
            atomType == kMINF ||
            atomType == kSTBL)
        {
            parseMp4(stream, atomLen);
        }
        else if (atomType == kSTSZ)
            parseStsz(stream, atomLen - 8);
        else if (atomType == kSTTS)
            parseStts(stream, atomLen - 8);
        else if (atomType == kMDAT)
        {
            mMdatOffset = mOffset;
            mMdatLength = atomLen - 8;
        }
        else
        {
            int len = sizeof(kSaveDataAtoms)/sizeof(kSaveDataAtoms[0]);
            for (int i = 0; i < len; i++)
            {
                int savedAtomType = kSaveDataAtoms[i];
                if (savedAtomType == atomType)
                {
                    char* data = new char[atomLen-8];
                    stream.read(data, atomLen - 8);
                    mOffset += atomLen - 8;
                    mAtomMap[atomType].data = data;
                }
            }
        }

        if (atomType == kSTSD)
            parseMp4aFromStsd();

        maxLen -= atomLen;
        int skipLen = atomLen - (mOffset - initialOffset);
        if (skipLen < 0)
            return -1;

        stream.seekg(skipLen, std::ios_base::cur);
        mOffset += skipLen;
    }
    return 1;
}
void CheapAAC::parseMdat(std::ifstream& stream, int maxLen)
{
    int initialOffset = mOffset;
    for (int i = 0; i < mNumFrames; i++)
    {
         mFrameOffsets[i] = mOffset;
         if (mOffset - initialOffset + mFrameLens[i] > maxLen - 8)
            mFrameGains[i] = 0;

         else
            readFrameAndComputeGain(stream, i);

         if (mFrameGains[i] < mMinGain)
            mMinGain = mFrameGains[i];

         if (mFrameGains[i] > mMaxGain)
            mMaxGain = mFrameGains[i];
    }
}

void CheapAAC::parseStsz(std::ifstream& stream, int maxLen)
{
    char stszHeader[12];
    stream.read(stszHeader, 12);
    mOffset += 12;
    mNumFrames = toInt((char*)&stszHeader[8]);

    mFrameOffsets = new int[mNumFrames];
    mFrameLens = new int[mNumFrames];
    mFrameGains = new int[mNumFrames];
    char* frameLenBytes = new char[4 * mNumFrames];
    stream.read(frameLenBytes, 4 * mNumFrames);
    mOffset += 4 * mNumFrames;

    for (int i = 0; i < mNumFrames; i++)
    {
        mFrameLens[i] = toInt((char*)&frameLenBytes[4*i]);
    }
    delete[] frameLenBytes;
}

void CheapAAC::parseStts(std::ifstream& stream, int maxLen)
{
    char sttsData[16];
    stream.read(sttsData, 16);
    mOffset += 16;

    mSamplesPerFrame = toInt((char*)&sttsData[12]);
}

void CheapAAC::parseMp4aFromStsd()
{
    char* stsdData = mAtomMap[kSTSD].data;
    mChannels =
        ((0xff & stsdData[32]) << 8) |
        ((0xff & stsdData[33]));

    mSampleRate =
        ((0xff & stsdData[40]) << 8) |
        ((0xff & stsdData[41]));
}

void CheapAAC::readFrameAndComputeGain(std::ifstream& stream, int frameIndex)
{
    if (mFrameLens[frameIndex] < 4)
    {
        mFrameGains[frameIndex] = 0;
        stream.seekg(mFrameLens[frameIndex], std::ios_base::cur);
        return;
    }

    int initialOffset = mOffset;
    char data[4];
    stream.read(data, 4);
    mOffset += 4;
    int idSynEle = (0xe0 & data[0]) >> 5;
    switch(idSynEle)
    {
    case 0:
        {
            // ID_SCE: mono
            int monoGain = ((0x01 & data[0]) << 7) | ((0xfe & data[1]) >>   1);
            mFrameGains[frameIndex] = monoGain;
        }
        break;
    case 1:
        {
             // ID_CPE: stereo
             int windowSequence = (0x60 & data[1]) >> 5;
             int windowShape = (0x10 & data[1]) >> 4;
             int maxSfb;
             int scaleFactorGrouping;
             int maskPresent;
             int startBit;

             if (windowSequence == 2)
             {
                 maxSfb = 0x0f & data[1];
                 scaleFactorGrouping = (0xfe & data[2]) >> 1;
                 maskPresent =
                    ((0x01 & data[2]) << 1) |
                    ((0x80 & data[3]) >> 7);
                 startBit = 25;
             }
             else
             {
                 maxSfb =
                    ((0x0f & data[1]) << 2) |
                    ((0xc0 & data[2]) >> 6);
                 scaleFactorGrouping = -1;
                 maskPresent = (0x18 & data[2]) >> 3;
                 startBit = 21;
             }
             if (maskPresent == 1)
             {
                 int sfgZeroBitCount = 0;
                 for (int b = 0; b < 7; b++)
                 {
                    if ((scaleFactorGrouping & (1 << b)) == 0)
                        sfgZeroBitCount++;
                 }

                 int numWindowGroups = 1 + sfgZeroBitCount;
                 int skip = maxSfb * numWindowGroups;
                 startBit += skip;
             }

             int bytesNeeded = 1 + ((startBit + 7) / 8);
             char* data2 = new char[bytesNeeded];

             data2[0] = data[0];
             data2[1] = data[1];
             data2[2] = data[2];
             data2[3] = data[3];

             stream.read(&data2[4], bytesNeeded - 4);
             mOffset += (bytesNeeded - 4);
             int firstChannelGain = 0;

             for (int b = 0; b < 8; b++)
             {
                 int b0 = (b + startBit) / 8;
                 int b1 = 7 - ((b + startBit) % 8);
                 int add = (((1 << b1) & data2[b0]) >> b1) << (7 - b);
                 firstChannelGain += add;
             }
             mFrameGains[frameIndex] = firstChannelGain;
             delete[] data2;
        }
        break;

    default:
        if (frameIndex > 0)
            mFrameGains[frameIndex] = mFrameGains[frameIndex - 1];
        else
            mFrameGains[frameIndex] = 0;
        break;
    }

    int skip = mFrameLens[frameIndex] - (mOffset - initialOffset);
    stream.seekg(skip, std::ios_base::cur);
    mOffset += skip;
}

void CheapAAC::startAtom(std::ofstream& out, int atomType)
{
    char atomHeader[8];
    int atomLen = mAtomMap[atomType].len;
    atomHeader[0] = ((atomLen >> 24) & 0xff);
    atomHeader[1] = ((atomLen >> 16) & 0xff);
    atomHeader[2] = ((atomLen >> 8) & 0xff);
    atomHeader[3] = (atomLen & 0xff);
    atomHeader[4] = ((atomType >> 24) & 0xff);
    atomHeader[5] = ((atomType >> 16) & 0xff);
    atomHeader[6] = ((atomType >> 8) & 0xff);
    atomHeader[7] = (atomType & 0xff);
    out.write(atomHeader, 8);
}

void CheapAAC::writeAtom(std::ofstream& out, int atomType)
{
    Atom& atom = mAtomMap[atomType];
    startAtom(out, atomType);
    out.write(atom.data, atom.len - 8);
}

void CheapAAC::setAtomData(int atomType, char* data, int len)
{
    Atom& atom = mAtomMap[atomType];
    atom.len = len + 8;
    atom.data = data;
}

void CheapAAC::writeFile(char* mInputFile, char* outputFile, int startFrame, int numFrames)
{
    std::ifstream in(mInputFile, std::ios::binary);
    std::ofstream out(outputFile, std::ios::binary);
    char data[] =
    {
        'M', '4', 'A', ' ',
        0, 0, 0, 0,
        'M', '4', 'A', ' ',
        'm', 'p', '4', '2',
        'i', 's', 'o', 'm',
        0, 0, 0, 0
    };
    setAtomData(kFTYP, data, sizeof(data));

    char data2[] =
    {
        0, 0, 0, 0,  // version / flags
        0, 0, 0, 1,  // entry count
        (char) ((numFrames >> 24) & 0xff),
        (char) ((numFrames >> 16) & 0xff),
        (char) ((numFrames >> 8) & 0xff),
        (char) (numFrames & 0xff),
        (char) ((mSamplesPerFrame >> 24) & 0xff),
        (char) ((mSamplesPerFrame >> 16) & 0xff),
        (char) ((mSamplesPerFrame >> 8) & 0xff),
        (char) (mSamplesPerFrame & 0xff)
    };
    setAtomData(kSTTS, data2, sizeof(data2));
    char data3[] =
    {
        0, 0, 0, 0,  // version / flags
        0, 0, 0, 1,  // entry count
        0, 0, 0, 1,  // first chunk
        (char) ((numFrames >> 24) & 0xff),
        (char) ((numFrames >> 16) & 0xff),
        (char) ((numFrames >> 8) & 0xff),
        (char) (numFrames & 0xff),
        0, 0, 0, 1  // Smaple desc index
    };
    setAtomData(kSTSC, data3, sizeof(data3));

    char* stszData = new char[12 + 4 * numFrames];
    memset(stszData, 0, 12 + 4 * numFrames);
    stszData[8] = ((numFrames >> 24) & 0xff);
    stszData[9] = ((numFrames >> 16) & 0xff);
    stszData[10] = ((numFrames >> 8) & 0xff);
    stszData[11] = (numFrames & 0xff);

    for (int i = 0; i < numFrames; i++)
    {
        stszData[12 + 4 * i] =
            ((mFrameLens[startFrame + i] >> 24) & 0xff);
        stszData[13 + 4 * i] =
            ((mFrameLens[startFrame + i] >> 16) & 0xff);
        stszData[14 + 4 * i] =
            ((mFrameLens[startFrame + i] >> 8) & 0xff);
        stszData[15 + 4 * i] =
            (mFrameLens[startFrame + i] & 0xff);
    }

    setAtomData(kSTSZ, stszData, 12+4*numFrames );
    int mdatOffset =
        144 +
        4 * numFrames +
        mAtomMap[kSTSD].len +
        mAtomMap[kSTSC].len +
        mAtomMap[kMVHD].len +
        mAtomMap[kTKHD].len +
        mAtomMap[kMDHD].len +
        mAtomMap[kHDLR].len +
        mAtomMap[kSMHD].len +
        mAtomMap[kDINF].len;

    char data4[] =
    {
        0, 0, 0, 0,  // version / flags
        0, 0, 0, 1,  // entry count
        (char) ((mdatOffset >> 24) & 0xff),
        (char) ((mdatOffset >> 16) & 0xff),
        (char) ((mdatOffset >> 8) & 0xff),
        (char) (mdatOffset & 0xff),
    };
    setAtomData(kSTCO, data4, sizeof(data4));

    mAtomMap[kSTBL].len =
        8 +
        mAtomMap[kSTSD].len +
        mAtomMap[kSTTS].len +
        mAtomMap[kSTSC].len +
        mAtomMap[kSTSZ].len +
        mAtomMap[kSTCO].len;

    mAtomMap[kMINF].len =
        8 +
        mAtomMap[kDINF].len +
        mAtomMap[kSMHD].len +
        mAtomMap[kSTBL].len;

    mAtomMap[kMDIA].len =
        8 +
        mAtomMap[kMDHD].len +
        mAtomMap[kHDLR].len +
        mAtomMap[kMINF].len;

    mAtomMap[kTRAK].len =
        8 +
        mAtomMap[kTKHD].len +
        mAtomMap[kMDIA].len;

    mAtomMap[kMOOV].len =
        8 +
        mAtomMap[kMVHD].len +
        mAtomMap[kTRAK].len;

    int mdatLen = 8;
    for (int i = 0; i < numFrames; i++)
    {
        mdatLen += mFrameLens[startFrame + i];
    }

    mAtomMap[kMDAT].len = mdatLen;

    writeAtom(out, kFTYP);
    startAtom(out, kMOOV);
    {
        writeAtom(out, kMVHD);
        startAtom(out, kTRAK);
        {
            writeAtom(out, kTKHD);
            startAtom(out, kMDIA);
            {
                writeAtom(out, kMDHD);
                writeAtom(out, kHDLR);
                startAtom(out, kMINF);
                {
                    writeAtom(out, kDINF);
                    writeAtom(out, kSMHD);
                    startAtom(out, kSTBL);
                    {
                        writeAtom(out, kSTSD);
                        writeAtom(out, kSTTS);
                        writeAtom(out, kSTSC);
                        writeAtom(out, kSTSZ);
                        writeAtom(out, kSTCO);
                    }
                }
            }
        }
    }
    startAtom(out, kMDAT);
    int maxFrameLen = 0;
    for (int i = 0; i < numFrames; i++)
    {
        if (mFrameLens[startFrame + i] > maxFrameLen)
            maxFrameLen = mFrameLens[startFrame + i];
    }

    char* buffer = new char[maxFrameLen];
    int pos = 0;
    for (int i = 0; i < numFrames; i++)
    {
        int skip = mFrameOffsets[startFrame + i] - pos;
        int len = mFrameLens[startFrame + i];
        if (skip < 0)
            continue;

        if (skip > 0)
        {
            in.seekg(skip, std::ios_base::beg);
            pos += skip;
        }

        in.read(buffer, len);
        out.write(buffer, len);
        pos += len;
    }

    delete[] stszData;
    delete[] buffer;
    in.close();
    out.close();
}
