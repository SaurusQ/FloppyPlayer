
#ifndef MIDIHEADERS_HPP
#define MIDIHEADERS_HPP

#include <iostream>
#include <string>
#include <vector>

#include "midiData.hpp"
#include "events.hpp"

class MThd;
class MTrk;

typedef struct
{
    unsigned int sequenceNumber;
    //unsigned int SMPTEoffset; not used at the moment TODO
    std::string copyrightNotice;
    std::string trackName;
    std::string intrumentName;
} Meta;

class MidiHeader
{
    public:
        void parseRawChunk(char *data, unsigned int pos);
        MThd makeMThd();
        MTrk makeMTrk();
        void print() const;
        //Getters and setters
        bool isValid() const {return isValid_;};
        uint32_t getTypeHas() const {return *(uint32_t*)chunkType_;};
        char* getChunkType() {return chunkType_;};
        uint32_t getLength() const {return length_;};
        uint32_t getPosition() const {return position_;};
    protected:
        char chunkType_[CHUNK_LEN + 1];     //<chunk type>
        uint32_t length_;                   //<length>
        unsigned int position_;             //Start of data in a chunk in a file
        bool isValid_ = true;
};

class MThd : public MidiHeader
{
    public:
        MThd() {};
        MThd(MidiHeader parent);
        void readMthdData(std::ifstream &midiFile);
        void printNotes();
        void print() const;
        //Getters and setters
        void setTempo(uint16_t tempo) {uTempo_ = tempo; };
        uint16_t getFormat() const { return format_; };
        uint16_t getNumTracks() const { return numTracks_; };
        uint16_t getDivision() const { return division_; };
        double getTempoUS() const { return uTempo_; };
        uint32_t getTempoNS() const { return uTempo_ * 1000; };
    protected:
        uint16_t format_;                   //<format>
        uint16_t numTracks_;                //<ntrks>
        uint16_t division_;                 //<division> how many ticks quarter note
        double uTempo_;                     //Tempo (us / tick)
};

class MTrk : public MidiHeader
{
    public:
        MTrk(MidiHeader parent);
        void readMtrkData(std::ifstream &midiFile);
        void print(bool notes = false) const;
        std::vector<Event*>& getEvents() {return events_;};
    private:
        Meta meta_;                         //Holds unrelevant metadata
        std::vector<Event*> events_;        //Every event in the track (for us)
};

#endif
