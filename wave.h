
#ifndef wave_h
#define wave_h
#include<stdio.h>
#include<vector>
#include<fstream>


#endif /* wave_h */

struct WaveHeader
{
    uint32_t chunk;          /* RIFF */
    uint32_t len0;           /* file size-8 (in bytes) */
    uint32_t riffType;       /* WAVE */

    uint32_t chunk2;         /* fmt */
    uint32_t len;            /* chunk length (16 bytes) */
    uint16_t format;         /* 1 is PCM, rest not important */
    uint16_t nchns;          /* channels */
    uint32_t rate;           /* sample rate */
    uint32_t aver;           /* nBlockAlign * srate */
    uint16_t nBlockAlign;    /* nchns * size / 8 */
    uint16_t bits;          /* size of each sample (8,16,32) */
    
    uint32_t chunk3;         /* DATA */
    uint32_t datasize;       /* sound data size in bytes */
};

static unsigned char RIFF_ID[4] = {'R','I','F','F'};
static unsigned char WAVE_ID[4] = {'W','A','V','E'};
static unsigned char FMT_ID[4] = {'f','m','t', ' '};
static unsigned char DATA_ID[4] = {'d','a','t','a'};

//template function for writing different bit depths
//last parameter is 1 for 32 bit floats
template <typename T>
bool write_header(std::string filename,
                  std::vector<T> data,
                  int32_t sr,
                  int16_t channels,
                  int isFloats)
{
    int32_t databytes = (int32_t) (data.size() * sizeof(T));
    int32_t precision = sizeof(T) * 8;
    
    std::ofstream file(filename, std::ios::binary);
    if(!file)
        return false;
    
    WaveHeader waveHead;
    
    waveHead.chunk = (*(uint32_t *) RIFF_ID);
    waveHead.len0 = databytes + sizeof(waveHead) - 8;
    waveHead.riffType = (*(uint32_t *) WAVE_ID);
    waveHead.chunk2 = (*(uint32_t *) FMT_ID);
    waveHead.len = 16;
    if(isFloats == 0)
        waveHead.format = 1;
    else if (isFloats == 1)
        waveHead.format = 3;        //format code for IEEE float is 0x0003
    waveHead.nchns = channels;
    waveHead.rate = sr;
    waveHead.aver = channels * precision * sr /8;
    waveHead.nBlockAlign = channels*precision/8;
    waveHead.bits = precision;
    waveHead.chunk3 = (*(uint32_t *) DATA_ID);
    waveHead.datasize = databytes;
    
    file.write((char*)&waveHead, sizeof(WaveHeader));
    
    file.write((char*)&data[0],databytes);
    
    return true;
}

