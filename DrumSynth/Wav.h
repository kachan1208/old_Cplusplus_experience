#pragma once
#include <stdio.h>

typedef unsigned char  byte;
typedef unsigned short uint16;
typedef unsigned int   uint32;

typedef struct WaveFormat
{
	uint16  wFormatTag;
	uint16  nChannels;
	uint32  nSamplesPerSec;
	uint32  nAvgBytesPerSec;
	uint16  nBlockAlign;
	uint16  wBitsPerSample;

	WaveFormat()
	{}

	WaveFormat(uint16 chan, uint32 rate, uint16 bits)
	{
		wFormatTag = 1;
		nChannels = chan;
		nSamplesPerSec = rate;
		wBitsPerSample = bits;
		nBlockAlign = chan * bits / 8;
		nAvgBytesPerSec = nBlockAlign * nSamplesPerSec;
	}

} WAVE_FORMAT;

#define WAVE_FORMAT_SIZE   ( 16 )

// WAVE format wFormatTag IDs
#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM    0x0001		
#endif


enum Result
{
	rOK = 0,
	rInvalid = -1,
};


class WavReader
{
public:
// Constructor/Destructor
	WavReader();
	~WavReader();

// Implementation
	Result      open(const char *path);
	int         extract(void *data, uint32 len);
	void        close();

// Attributes
	/// returns sample count
	uint32      length() const;
	uint32      sampleRate() const { return _format.nSamplesPerSec; }
	WAVE_FORMAT format() const     { return _format; }

protected:
	int         read(void *data, uint32 len);
	int         read(const char *marker);
	int         read(uint32 &value);

private:
// Members
	FILE*       _fd;
	WAVE_FORMAT _format;
	uint32      _rawDataLen;
	uint32		_rawDataPos;
};



class WavWriter
{
public:
// Constructor/Destructor
	WavWriter();
	~WavWriter();

// Implementation
	int    open(const char *path, WAVE_FORMAT format);
	int    write(void *data, uint32 len);
	void   close();

protected:
	int    write(const char *marker);
	int    write(uint32 value);

private:
// Members
	FILE*       _fd;
	uint32		_dataChunkPos;
};
