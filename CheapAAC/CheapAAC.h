#include <string.h>
#include <fstream>
#include <map>

class CheapAAC
{
public:
//Constructor/Destructor
    CheapAAC();
    ~CheapAAC();

//Attributes
    int getNumFrames() {
       return mNumFrames;
    }

    int getSamplesPerFrame() {
        return mSamplesPerFrame;
    }

    int* getFrameOffsets() {
        return mFrameOffsets;
    }

    int* getFrameLens() {
        return mFrameLens;
    }

    int* getFrameGains() {
        return mFrameGains;
    }

    int getFileSizeBytes() {
        return mFileSize;
    }

    int getAvgBitrateKbps() {
        return mFileSize / (mNumFrames * mSamplesPerFrame);
    }

    int getSampleRate() {
        return mSampleRate;
    }

    int getChannels() {
        return mChannels;
    }

    const char* getFiletype() {
        return "AAC";
    }
//implementation
    std::string atomToString(int atomType);
    int readFile(const char* inputFile);
    int parseMp4(std::ifstream& stream, int maxLen);
    void parseMdat(std::ifstream& stream, int maxLen);
    void parseStsz(std::ifstream& stream, int maxLen);
    void parseStts(std::ifstream& stream, int maxLen);
    void parseMp4aFromStsd();
    void readFrameAndComputeGain(std::ifstream& stream, int frameIndex);
    void startAtom(std::ofstream& out, int atomType);
    void writeAtom(std::ofstream& out, int atomType);
    void setAtomData (int atomType, char* data, int len);
    void writeFile(char* mInputFile , char* outputFile, int startFrame, int numFrames);

private:
    int mNumFrames;
    int* mFrameOffsets;
    int* mFrameLens;
    int* mFrameGains;
    int mFileSize;


// Member variables containing sound file info
    int mBitrate;
    int mSampleRate;
    int mChannels;
    int mSamplesPerFrame;

// Member variables used only while initially parsing the file
    int mOffset;
    int mMinGain;
    int mMaxGain;
    int mMdatOffset;
    int mMdatLength;

    struct Atom
    {
        int    start;
        int    len;
        char*  data;
        Atom()
            : start(0)
            , len(0)
            , data(0)
       {}
    };


    typedef std::map<int, Atom> AtomMap;
    AtomMap mAtomMap;
};
