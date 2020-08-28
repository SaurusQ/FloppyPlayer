
#ifndef EVENT_HPP
#define EVENT_HPP

#include <iostream>
#include <fstream>

#include "Serial.hpp"
#include "midiCom.hpp"
#include "midiData.hpp"
#include "utils.hpp"

class Event
{
    public:
        virtual bool execute(Serial &usbCom, SongConf &conf, uint8_t driveNum) = 0;// {printf("Event\n"); return true;};
        uint32_t getDeltaTime() const { return deltaTime_; };
        EventType getType() const { return type_; };
    protected:
        EventType type_;
        uint32_t deltaTime_;
};

class Note : public Event
{
    public:
        Note(uint8_t statusByte, std::ifstream &midiFile, int64_t &bytesLeft, uint32_t time);
        bool execute(Serial &usbCom, SongConf &conf, uint8_t driveNum);
        uint8_t getChannel() const { return channel_; };
        uint8_t getNote() const { return note_; };
        uint8_t getVelocity() const { return velocity_; };
    private:
        uint8_t note_;
        uint8_t velocity_;
        uint8_t channel_;
};

class MetaEvent : public Event
{
    public:
        MetaEvent(EventType type, uint8_t *data, uint8_t len, uint32_t deltaTime);
        ~MetaEvent();
        bool execute(Serial &usbCom, SongConf &conf, uint8_t driveNum);
    private:
        uint8_t *data_;
        uint8_t len_;
};

#endif
