
#ifndef MIDIPLAYER_HPP
#define MIDIPLAYER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <chrono>
#include <memory>

#include "midiData.hpp"
#include "midiHeaders.hpp"
#include "Serial.hpp"
#include "utils.hpp"

class MidiPlayer
{
    public:
        MidiPlayer(std::string fileName);
        void parse();
        void resetPlay(Serial *usbCom);
        void configurePlay();
        void getChannels();
        void getTracks();
        void playUSB();
        bool isValid();
    //Debug functions
        void printData();
    private:
        PlayConf configuration_;
        SongConf curSongStat_;
        std::string fileName_;
        MThd mthd_;
        std::vector<MTrk> trackData_;
        std::vector<MidiHeader> otherChunks_;
        unsigned int undefinedChunks_ = 0;
};

#endif
