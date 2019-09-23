
#ifndef MIDIPLAYER_HPP
#define MIDIPLAYER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <chrono>
#include <memory>
#include <queue>
#include <tuple>

#include "midiData.hpp"
#include "midiHeaders.hpp"
#include "Serial.hpp"
#include "utils.hpp"
#include "events.hpp"

struct EventData {
    uint64_t tick;
    uint16_t trackNum;
    Event *pEvent;
};

class MidiPlayer
{
    public:
        MidiPlayer(std::string fileName);
        void parse();                       //Parse MIDI file
        void resetPlay(Serial *usbCom);     //Reset current tempo and other velues to the starting condition, reset also usbCom when provided
        void configurePlay();
        void playUSB();
        bool isValid() const;
    //Debug functions
        void printData();
    private:
        SongConf curSongStat_;
        std::string fileName_;
        MThd mthd_;
        std::vector<MTrk> trackData_;
        std::vector<MidiHeader> otherChunks_;
        unsigned int undefinedChunks_ = 0;
};

#endif
