
#include "events.hpp"

Note::Note(uint8_t statusByte, std::ifstream &midiFile, int64_t &bytesLeft, uint32_t deltaTime)
{
    uint8_t data[2];
    channel_ = statusByte & 0x0F;
    deltaTime_ = deltaTime;
    //Ignore all events except Note Off/On
    switch(statusByte & 0xF0)
    {
        case 0xC0:
        case 0xD0:      //Ignore one length events
            type_ = E_IGNORE;
            midiFile.ignore(1);
            bytesLeft -= 1;
            break;
        case 0x80:      //Note off
            type_ = E_NOTE;
            note_ = midiFile.get();
            velocity_ = 0; midiFile.ignore(1);
            bytesLeft -= 2;
            break;
        case 0x90:      //Note on
            type_ = E_NOTE;
            note_ = midiFile.get();
            velocity_ = midiFile.get();
            bytesLeft -= 2;
            break;
        case 0xE0:      //Pitch bend, ignore for now
        default:        //Ignore other events
            midiFile.ignore(2);
            type_ = E_IGNORE;
            bytesLeft -= 2;
            break;
    }
}

bool Note::execute(Serial &usbCom, SongConf &conf)
{
    printf("Note: %d, Velocity: %d\n", this->note_, this->velocity_);
    //HIGH filter remove TODO
    //note_ -= 17;
    if(note_ > 67)
    {
        note_ = 67;
    }
    int len = 3;
    uint8_t buff[len] = {E_NOTE, 0/*channel for controller*/, note_};//TODO type
    usbCom.writeData(buff, len);
    return true;
}

MetaEvent::MetaEvent(EventType type, uint8_t *data, uint8_t len, uint32_t deltaTime)
{
    printf("Creating metaevent with p: %p\n", this);
    type_ = type;
    deltaTime_ = deltaTime;
    data_ = new uint8_t[len];
    memcpy(data_, data, len);
    len_ = len;
}

MetaEvent::~MetaEvent()
{
    delete data_;
}

bool MetaEvent::execute(Serial &usbCom, SongConf &conf)
{
    printf("Meta\n");
    switch(type_)
    {
        case E_TEMPO: //Tempo change event
            {
                int msTempo = 0;
                printf("len: %d\n", this->len_);
                printf("%02x, %02x, %02x", data_[0], data_[1], data_[2]);
                memcpy(&msTempo, this->data_, this->len_);
                conf.nsTempo = static_cast<uint32_t>(my_ntohl(msTempo << 8) * (double)1000 / (double)conf.division);
            }
            break;
        case E_TIME_SIGN:
            std::cerr << "ERROR: Time signature not implemented" << std::endl;
            break;
        default:
            std::cerr << "Meta not implemented" << std::endl;
            break;
    }
    return true;
}
