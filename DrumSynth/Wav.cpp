//#ifdef WIN32
//#include "stdafx.h"
#include <string.h>
//#endif
#include <stdlib.h>
#include "Wav.h"

//BE:#define	MAKE_MARKER(a, b, c, d)    (((a) << 24) | ((b) << 16) | ((c) << 8) | (d))
#define	MAKE_MARKER(a, b, c, d)    ((a) | ((b) << 8) | ((c) << 16) | ((d) << 24))

static const uint32 MARKER_FMT  = MAKE_MARKER ('f', 'm', 't', ' ');
static const uint32 MARKER_DATA = MAKE_MARKER ('d', 'a', 't', 'a');



WavReader::WavReader()
	: _fd(NULL)
	, _rawDataLen(0)
	, _rawDataPos(0)
{
	memset(&_format, 0, WAVE_FORMAT_SIZE);
}

WavReader::~WavReader()
{
	close();
}


Result WavReader::open(const char *path)
{
	_fd = fopen(path, "rb");
	if (_fd == NULL)
		return rInvalid;

	// read RIFF
	if (read("RIFF") != 0)
		return rInvalid;
	uint32 riffSize = 0;
	if (read(riffSize) != 0)
		return rInvalid;

	// read WAVE format 
	if (read("WAVE") != 0)
		return rInvalid;


	while (!feof(_fd))
	{
		uint32 marker = 0;
		int ret = read(&marker, sizeof(marker));
		if (ret < sizeof(marker))
			return rInvalid;

		if (marker == MARKER_FMT)
		{
			uint32 fmtSize = 0;
			if (read(fmtSize) != 0)
				return rInvalid;
			if (fmtSize != WAVE_FORMAT_SIZE)
				return rInvalid;

			memset(&_format, 0, WAVE_FORMAT_SIZE);
			if (read(&_format, WAVE_FORMAT_SIZE) != WAVE_FORMAT_SIZE)
				return rInvalid;
			if (_format.wFormatTag != WAVE_FORMAT_PCM)
				return rInvalid;
		}
		else if (marker == MARKER_DATA)
		{
			break;
		}
		else
		{
			// skip unknown chunk
			uint32 len = 0;
			if (read(len) != 0)
				return rInvalid;
			int ret = fseek(_fd, len, SEEK_CUR);
			if (ret)
				return rInvalid;
		}
	}

	// read data marker size
	_rawDataPos = _rawDataLen = 0;
	if (read(_rawDataLen) != 0)
		return rInvalid;

	// ready to read data
	return rOK;
}

int WavReader::extract(void *data, uint32 len)
{
	int rest = _rawDataLen - _rawDataPos;
	if (rest < 1)
		return 0;
	if (rest < len)
		len = rest;
	int ret = read(data, len);
	if (ret > 0)
		_rawDataPos += ret;
	return ret;
}

void WavReader::close()
{
	if (_fd)
	{
		fclose(_fd);
		_fd = NULL;
	}
}

uint32 WavReader::length() const
{
	if (_fd == NULL)
		return -1;
	uint32 width = _format.nChannels * _format.wBitsPerSample / 8;
	uint32 real  = _rawDataLen / width;
	return real;
}



int WavReader::read(void *data, uint32 len)
{
	//uint16 *buf = reinterpret_cast<uint16 *>(data);
	return fread(data, 1, len, _fd);
}

int WavReader::read(const char *marker)
{
	int rc    = -1;
	int len   = strlen(marker);
	char *buf = new char[len + 1];
	memset(buf, 0, len + 1);	
	int bytesRead = fread(buf, 1, len, _fd);
	if (bytesRead == len)
		rc = (strncmp(buf, marker, len) == 0) ? 0 : -1;
	delete [] buf;
	return rc;
}

int WavReader::read(uint32 &value)
{
	return (fread(&value, 1, 4, _fd) != 4);
}





WavWriter::WavWriter()
	: _fd(NULL)
	, _dataChunkPos(0)
{
}

WavWriter::~WavWriter()
{
}

int WavWriter::open(const char *path, WAVE_FORMAT format)
{
	if (format.wFormatTag != WAVE_FORMAT_PCM)
		return -1;

	_fd = fopen(path, "wb+");
	if (_fd == NULL)
		return -1;

	write("RIFF");
	write(uint32(0));
	write("WAVEfmt ");
	write(WAVE_FORMAT_SIZE);
	write(&format, WAVE_FORMAT_SIZE);

	_dataChunkPos = ftell(_fd);
	write("data");
	write(uint32(0));
	
	return 0;
}

int WavWriter::write(void *data, uint32 len)
{
	return fwrite(data, 1, len, _fd);
}

void WavWriter::close()
{
	if (_fd == NULL)
		return;

	uint32 pos = ftell(_fd);
	// "RIFF" size
	fseek(_fd, 4, SEEK_SET);
	write(pos - 4);
	// "data" size
	fseek(_fd, _dataChunkPos + 4, SEEK_SET);
	uint32 size = pos - _dataChunkPos - 8; 
	write(size);
	fclose(_fd);
}

int WavWriter::write(const char *marker)
{
	int len = strlen(marker);
	return fwrite(marker, 1, len, _fd);
}

int WavWriter::write(uint32 value)
{
	return fwrite(&value, 1, sizeof(uint32), _fd);
}
