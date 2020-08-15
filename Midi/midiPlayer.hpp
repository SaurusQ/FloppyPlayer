
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
#include <atomic>
#include <mutex>

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
        MidiPlayer(std::string usbPort);
        void reset();
        void parse(std::string filename);                       //Parse MIDI file
        void resetPlay(Serial *usbCom);     //Reset current tempo and other velues to the starting condition, reset also usbCom when provided
        void configurePlay();
        void moveNotes(uint8_t diff);
        void setTrackFilter(std::vector<bool> filters);
        void playUSB();
        void pause() { if(!isPaused_) playMutex_.lock(); isPaused_ = true; }
        void play() { if(isPaused_) playMutex_.unlock(); isPaused_ = false; }
        void endSong() { if(isPaused_) playMutex_.unlock(); endPlay_ = true;}
        void setUSBport(std::string usbPort);
        bool isValid() const;
    //Debug functions
        void printData() const;
    private:
        mutable std::mutex playMutex_;
        mutable std::atomic<bool> endPlay_;
        bool isPaused_;
        bool filters_[9] = {false};

        SongConf curSongStat_;
        std::string fileName_;
        std::string usbPort_;
        MThd mthd_;
        std::vector<MTrk> trackData_;
        std::vector<MidiHeader> otherChunks_;
        unsigned int undefinedChunks_ = 0;
};

#endif
