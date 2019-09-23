
#include <fstream>

#include "midiHeaders.hpp"

#include "utils.hpp"

void MidiHeader::parseRawChunk(char *data, unsigned int pos)
{
    memcpy(chunkType_, data, CHUNK_LEN);
    chunkType_[CHUNK_LEN] = '\0';
    length_ = my_ntohl(*(uint32_t*)(data + CHUNK_LEN));
    position_ = pos;
}

void MidiHeader::print()
{
    std::cout << chunkType_ << "-> Lenght: " << length_ << std::endl;
}

//MThd------------------------------------------------------------------------------

MThd::MThd(MidiHeader parent)
{
    //Copy parents data to the child
    memcpy(chunkType_, parent.getChunkType(), CHUNK_LEN + 1);
    length_ = parent.getLength();
    position_ = parent.getPosition();
    if(length_ != MTHD_LEN)
    {
        std::cerr << "MThd length is invalid." << std::endl;
        isValid_ = false;
    }
}

void MThd::readMthdData(std::ifstream &midiFile)
{
    //Read additional information on MThd
    char data[MTHD_LEN];
    midiFile.read(data, MTHD_LEN);
    format_     = my_ntohs(*(uint16_t*)data);
    numTracks_  = my_ntohs(*(uint16_t*)(data + 2));
    uint16_t timeData   = my_ntohs(*(uint16_t*)(data + 4));
    if(!(timeData & 0x8000))
    {
        //Default tempo / tempo is set in the events
        division_ = timeData;
        uTempo_ = DEFAULT_TEMPO;
    }
    else
    {
        uint8_t fps;
        uint8_t beatsPerFrame = timeData & 0xFF00;
        switch((int8_t)(timeData & 0x00FF))
        {
            case -24:
            case -25:
            case -29:
            case -30:
                fps = abs(timeData & 0x00FF);
                break;
            default:
                std::cerr << "ERROR: Unrecognized frametime." << std::endl;
                return;
        }
        division_ = FPS_DIVISION;
        uTempo_ = 1000000u / (fps * beatsPerFrame);
    }
    
}

void MThd::print()
{
    std::cout << chunkType_ << "-> Lenght: " << length_ << " Format: " << 
        format_ << " Tracks: " << numTracks_ << " Division: " << division_ << std::endl;
}

//MTrk------------------------------------------------------------------------------

MTrk::MTrk(MidiHeader parent)
{
    //Copy parents data to the child
    memcpy(chunkType_, parent.getChunkType(), CHUNK_LEN + 1);
    length_ = parent.getLength();
    position_ = parent.getPosition();
    //Set the meta event data to default values
    meta_.sequenceNumber = (unsigned int)-1;
    meta_.trackName = "";
    meta_.intrumentName = "";
    meta_.copyrightNotice = "";
}

void MTrk::readMtrkData(std::ifstream &midiFile)
{
    int64_t bytesLeft = length_;
    uint32_t deltaTime = 0;
    bool endOfTrack = false;
    uint8_t runningStatus = 0; //0 = invalid running status
    uint8_t statusByte;

    while(!endOfTrack)
    {
        if(bytesLeft <= 0)
        {
            //Parsing failed
            break;
        }
        if(midiFile.fail() || midiFile.eof())
        {
            std::cerr << "ERROR: fail or eof reached." << std::endl;
            break;
        }
        //Handle <delta-time>
        deltaTime = readVarLenArg(midiFile, bytesLeft);

        /* Handle
         * <sysex event> (variable length)
         * <meta-event>  (variable length)
         * <MIDI event>  (0xCn and 0xDn are one byte shorter)
         */

        //Handle running status (only works with midi events)
        statusByte  = midiFile.peek();
        if(statusByte & 0x80)
        {
            midiFile.ignore(1);
            bytesLeft--;
        }
        else if(!runningStatus)
        {
            std::cerr << "ERROR: Running status requested with invalid status." << std::endl;
            break;
        }
        else
        {
            //std::cout << "Running status byte: 0x" << std::hex << 
            //            (int)statusByte << " new: 0x" << (int)runningStatus << std::dec << std::endl;
            statusByte = runningStatus;
        }
        
        switch(statusByte)
        {
            //<sysex event> (invalidates running status)
            case 0xF0:
            {
                runningStatus = 0;
                uint8_t data;
                do
                {
                    data = midiFile.get();
                    bytesLeft--;
                } while(data != 0xF7);
                break;
            }
            case 0xF7:
            {
                /*
                uint32_t varLen = readVarLenArg(midiFile, bytesLeft);
                //Ignore <sysex event> //TODO
                midiFile.ignore(varLen);
                bytesLeft -= varLen;*/
                runningStatus = 0;
                std::cerr << "Found escape sequence. How to handle???" << std::endl;
                break;
            }
            //System common messages
            case 0xF1 ... 0xF6:
            {
                runningStatus = 0;
                std::cerr << "System category not implemented." << std::endl;
            }
            //RealTime Category (doesn't invalidate running status)//TODO???
            case 0xF8 ... 0xFE:
            {
                runningStatus = 0;
                std::cerr << "Realtime category not implemented." << std::endl;
            }
            //<meta-event>
            case 0xFF:
            {
                runningStatus = 0;
                int typeByte = midiFile.get();
                int lenByte = midiFile.get();
                //Read data
                char *data = new char[lenByte + 1];
                data[lenByte] = '\0'; //Nullterminated for text handling
                midiFile.read(data, lenByte);
                bytesLeft -= lenByte + 2;
                switch(typeByte)
                {
                    case 0x00:  //Sequence number
                        meta_.sequenceNumber = my_ntohs(*(uint16_t*)data);
                        break;
                    case 0x01:  //Text event
                        break;  //TODO
                    case 0x02:  //Copyright notice
                        meta_.copyrightNotice = data;
                        break;
                    case 0x03:  //Sequence/track name
                        meta_.trackName = data;
                        break;
                    case 0x04:  //Instrument name
                        meta_.intrumentName = data;
                        break;
                    case 0x05:  //Lyric
                        break;  //TODO
                    case 0x06:  //Marker
                        break;  //TODO
                    case 0x07:  //Cue point (tells something is happening like dance is changing)
                        break;  //TODO
                    case 0x20:  //MIDI channel prefix (include this channel in the following sysex and
                                //meta events before next MIDI event)
                        break;
                    case 0x2F:  //End of track
                        {
                            endOfTrack = true;
                            events_.push_back(new MetaEvent(E_END_OF_TRACK, (uint8_t*)data, lenByte, deltaTime));
                        }
                        break;
                    case 0x51:  //Set tempo
                        {
                            events_.push_back(new MetaEvent(E_TEMPO, (uint8_t*)data, lenByte, deltaTime));
                        }
                        break;                                          //TODOtempo
                    case 0x54:  //SMPTE Offset (tells when this track starts to play)
                        std::cerr << "ERROR: SMPTE offset has not yet been implemeted." << std::endl;
                        break;                                          //TODO
                    case 0x58:  //Time signature (dik)
                        {
                            events_.push_back(new MetaEvent(E_TIME_SIGN, (uint8_t*)data, lenByte, deltaTime));
                        }
                        break;                                          //TODO
                    case 0x59:  //Key signature
                        break;                                          //TODO
                    case 0x7F:  //Sequencer specific meta event
                        break;
                    default:
                        std::cout << "Unrecognized meta event found" << std::endl;
                        break;
                }
                delete data;
                break;
            }
            //<MIDI event>
            case 0x80 ... 0xEF:
            { 
                Note *newNote = new Note(statusByte, midiFile, bytesLeft, deltaTime);
                if(newNote->getType() != E_IGNORE)
                {
                    events_.push_back(newNote);
                }
                //Running status is only implemented on midi messages
                runningStatus = statusByte;
                break;
            }
            default:
            {
                endOfTrack = true;
                std::cerr << "ERROR: Unrecognized midi event." << std::endl;
            }
        }
    }
    if(bytesLeft != 0)
    {
        std::cerr << "ERROR: Event parsing failed." << std::endl;
    }
}

void MTrk::print(bool notes)
{
    MidiHeader::print();
    std::cout << "  Sequence Number:  " << meta_.sequenceNumber
            << "\n  Track name:       " << meta_.trackName
            << "\n  Instrument:       " << meta_.intrumentName
            << "\n  Copyright notice: " << meta_.copyrightNotice << std::endl;

    if(notes)
    {
        for(auto it = events_.begin(); it != events_.end(); it++)
        {
            //printf(" C%-2d.%-3d ", it->getChannel(), it->getNote());
        }
    }
    std::cout << std::endl;
}