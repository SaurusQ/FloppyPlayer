
#include "midiPlayer.hpp"

MidiPlayer::MidiPlayer(std::string fileName)
{
    fileName_ = fileName;
}

void MidiPlayer::parse()
{
    std::ifstream midiFile(fileName_, std::ios::binary);
    if(midiFile.fail())
    {
        std::cerr << "Failed to open: " << fileName_ << std::endl;
        return;
    }

    //Get chunks
    char curData[CHUNK_LEN + LENGTH_LEN];
    MidiHeader curHeader;
    bool mthdFound = false;

    while(!midiFile.eof())
    {
        midiFile.read((char*)curData, CHUNK_LEN + LENGTH_LEN);
        if(midiFile.eof())
        {
            break;
        }
        curHeader.parseRawChunk(curData, midiFile.tellg());
        switch(curHeader.getTypeHas())
        {
            case MTHD_HAS:
            {
                if(mthdFound)   //Only MThd should be present
                {
                    std::cerr << "ERROR: Multiple MThd headers." << std::endl;
                    midiFile.ignore(mthd_.getLength());
                    continue;
                }
                mthdFound = true;
                mthd_ = MThd(curHeader);
                if(mthd_.isValid())
                {
                    mthd_.readMthdData(midiFile);
                }
                else
                {
                    midiFile.ignore(mthd_.getLength());
                }
                break;
            }
            case MTRK_HAS:
            {
                MTrk newTrack(curHeader);
                newTrack.readMtrkData(midiFile);
                trackData_.push_back(newTrack);
                break;
            }
            default:
            {
                otherChunks_.push_back(curHeader);
                undefinedChunks_++;
                //Ignore data in undefined chunks
                midiFile.ignore(curHeader.getLength());
                break;
            }
        }
    }
    midiFile.close();
}

void MidiPlayer::resetPlay(Serial *usbCom)
{
    //Reset drives only if we are instructed
    if(usbCom != nullptr)
    {
        const uint8_t buff[3] = {RESET_ALL, 0, 0};
        //Reset drive
        usbCom->writeData(buff, 3);//TODO read confirmation???
    }
    //Set curSongStat to starting values
    this->curSongStat_.nsTempo = mthd_.getTempoNS();
    this->curSongStat_.division = mthd_.getDivision();
}

void MidiPlayer::configurePlay()
{
    std::cout << "This file is format: ";

    switch(mthd_.getFormat())
    {
        case SINGLE_TRACK:
            std::cout << "single track" << std::endl;
            //Add that single track to out configuration
            break;
        case SIM_TRACKS:
            std::cout << "Multiple simultaneous tracks" << std::endl;
            //First track is allways the tempo map
            break;
        case IND_TRACKS:
            std::cout << "multiple independant tracks" << std::endl;
            std::cout << "ERROR: independent tracks not implemented" << std::endl;
            break;
        default:
            std::cout << "ERROR: format not recognized" << std::endl; 
    }
}

void MidiPlayer::playUSB()
{
    bool running = true;
    uint64_t tick = 0;

    //TODO connection configuration
    Serial usbCom("COM7", 9600);
    if(!usbCom.isConnected())
    {
        std::cerr << "ERROR: usb not connected." << std::endl;
        return;
    }

    //Reset song configurations to the starting position
    this->resetPlay(nullptr);
    
    //Initialize the queue
    std::vector<std::vector<Event*>::const_iterator> tracks;
    auto cmp = [](EventData left, EventData right) { return left.tick > right.tick; };
    std::priority_queue<EventData, std::vector<EventData>, decltype(cmp)> eventQueue(cmp);
    tracks.reserve(32);
    uint16_t trackNum = 0;
    for(auto it = this->trackData_.begin();
        it != this->trackData_.end();
        it++)
    {
        tracks.push_back(it->getEvents().begin());
        if((*tracks.back())->getType() != E_END_OF_TRACK) //Don't push empty track to the queue
        {
            EventData data = {(*tracks.back())->getDeltaTime(), trackNum, *tracks.back()};
            eventQueue.push(data);
        }
        trackNum++;
    }

    std::cout << "This midi file looks like it has: " << eventQueue.size() << " tracks" << std::endl;

    std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point curTime;
    std::vector<EventData> curTickEvents;
    curTickEvents.reserve(100);

    while(eventQueue.size() != 0 && usbCom.isConnected())
    {
        curTime = std::chrono::high_resolution_clock::now();
        if(std::chrono::nanoseconds(static_cast<int>(curSongStat_.nsTempo)) 
            <= std::chrono::duration_cast<std::chrono::nanoseconds>(curTime - startTime))
        {
            if(eventQueue.top().tick == tick) //Check next event time
            {
                auto a = eventQueue;
                while(a.size() != 0)
                {
                    std::cout << a.top().tick << " ";
                    a.pop();
                }
                std::cout << std::endl;
                do
                {
                    curTickEvents.push_back(eventQueue.top());
                    //If more events with 0 delta time are in the track add those to the curTickEvents
                    //else add next track event to queue
                    EventData data;
                    for(auto &it = tracks[eventQueue.top().trackNum]; (*it)->getType() != E_END_OF_TRACK; it++)
                    {
                        data = {tick + (*it)->getDeltaTime(), eventQueue.top().trackNum, *it};
                        if(data.tick == tick)
                        {
                            curTickEvents.push_back(data);
                        }
                        else
                        {
                            it++;
                            break;
                        }
                    }
                    eventQueue.pop();
                    eventQueue.push(data);
                } while(eventQueue.top().tick == tick);

                //TODO filter events

                //TODO make configuration affect here to destination, note and channel

                //Temporary straight play
                for(auto i : curTickEvents)
                {
                    if(i.trackNum == 1 || i.trackNum == 1 || i.trackNum == 0)
                        i.pEvent->execute(usbCom, curSongStat_);
                }
                curTickEvents.clear();
            }

            //Update the start time of the next tick
            startTime += std::chrono::nanoseconds(static_cast<int>(curSongStat_.nsTempo));
            tick++;
/*
            //Debug check that the ticks are not late
            curTime = std::chrono::high_resolution_clock::now();
            if(std::chrono::nanoseconds(static_cast<int>(curSongStat_.nsTempo)) 
            <= std::chrono::duration_cast<std::chrono::nanoseconds>(curTime - startTime))
            {
                std::cerr << "ERROR: ticks are late!" << std::endl;
            }*/
        }
    }
    //Send reset signal
    this->resetPlay(&usbCom);
}

//Debug functions--------------------------------------------------------------------------------

void MidiPlayer::printData()
{
    //Print header information
    mthd_.print();
    //Print track information
    for(std::vector<MTrk>::iterator it = trackData_.begin(); it != trackData_.end(); it++)
    {
        it->print(true);
    }
}