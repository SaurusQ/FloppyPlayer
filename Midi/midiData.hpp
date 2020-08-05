
#ifndef MIDIDATA_HPP
#define MIDIDATA_HPP

#include <vector>

#define MTHD_LEN        6u
#define CHUNK_LEN       4u
#define LENGTH_LEN      4u
#define DELTA_MAX_LEN   4u

#define CHANNEL_NUM 16u

#define MTHD_HAS 0x6468544D
#define MTRK_HAS 0x6B72544D

//Types
#define SINGLE_TRACK    0u
#define SIM_TRACKS      1u
#define IND_TRACKS      2u

#define DEFAULT_TEMPO       500000u //us / tick
#define FPS_DIVISION        0x250u

class MTrk;

typedef struct
{
    std::vector<MTrk*> tracks;
    bool channels[16];
} PlayConf;

typedef struct
{
    uint32_t nsTempo;
    uint32_t division;
    int16_t noteDiff;
} SongConf;

#endif
