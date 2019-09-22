
#include "midiPlayer.hpp"

MidiPlayer::MidiPlayer(std::string fileName)
{
    fileName_ = fileName;
    //Reset configuration
    memset(&configuration_, 0, sizeof(configuration_));
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
            this->configuration_.tracks.push_back(&this->trackData_[0]);
            this->getChannels();
            break;
        case SIM_TRACKS:
            std::cout << "Multiple simultaneous tracks" << std::endl;
            //First track is allways the tempo map
            this->configuration_.tracks.push_back(&this->trackData_[0]);
            this->getTracks();
            this->getChannels();
            break;
        case IND_TRACKS:
            std::cout << "multiple independant tracks" << std::endl;
            std::cout << "ERROR: independent tracks not implemented" << std::endl;
            break;
        default:
            std::cout << "ERROR: format not recognized" << std::endl; 
    }
}

void MidiPlayer::getChannels()
{
    std::cout << "Which channels would you like to play?" << std::endl;
    int input;
    std::cin >> input;
    while(input >= 0 && input < 16 || input == -1)
    {
        if(input == -1)//Set all channels active
        {
            for(int i = 0; i < sizeof(configuration_.channels); i++)
            {
                this->configuration_.channels[i] = true;
            }
            break;
        }
        this->configuration_.channels[input] = true;
        std::cin >> input;
    }
}

void MidiPlayer::getTracks()
{
    std::cout << "Which tracks would you like to play?" << std::endl;
    int input;
    std::cin >> input;
    while(input >= 0)
    {
        if(input >= this->trackData_.size())
        {
            std::cout << "Index out of bounds" << std::endl;
        }
        else
        {
            this->configuration_.tracks.push_back(&this->trackData_[input]);
        }
        std::cin >> input;
    }
}

void MidiPlayer::playUSB()
{
    bool running = true;
    uint64_t tick = 0;
    unsigned int numTracks = this->configuration_.tracks.size();
    unsigned int stoppedTracks = 0;

    Serial usbCom("COM7", 9600);
    if(!usbCom.isConnected())
    {
        std::cerr << "ERROR: usb not connected." << std::endl;
        return;
    }

    this->resetPlay(nullptr);
    
    std::vector<std::vector<Event*>::const_iterator> tracksToPlay;
    std::vector<uint64_t> nextEventTickCnts;

    tracksToPlay.reserve(numTracks);

    int i = 0;
    for(std::vector<MTrk*>::iterator it = this->configuration_.tracks.begin();
        it != this->configuration_.tracks.end();
        it++)
    {
        tracksToPlay.push_back((*it)->getEvents().begin());
        nextEventTickCnts.push_back((*tracksToPlay[i])->getDeltaTime());
        i++;
    }

    std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point curTime;

    while(numTracks)
    {
        curTime = std::chrono::high_resolution_clock::now();
        if(std::chrono::nanoseconds(static_cast<int>(curSongStat_.nsTempo)) 
            <= std::chrono::duration_cast<std::chrono::nanoseconds>(curTime - startTime))
        {
            for(int i = 0; i < numTracks; i++)
            {
                if(nextEventTickCnts[i] <= tick && nextEventTickCnts[i] != 0xFFFFFFFFFFFFFFFF)
                {
                    //Execute Event
                    (*tracksToPlay[i])->execute(usbCom, this->curSongStat_);
                    (*tracksToPlay[i])->getType();
                    tracksToPlay[i]++;
                    if((*tracksToPlay[i])->getType() == E_END_OF_TRACK)
                    {
                        //Tracks has ended, remove it
                        numTracks--;//TODO swapping might delay event for one tick
                        tracksToPlay[i] = tracksToPlay[numTracks];

                    }
                    nextEventTickCnts[i] = (*tracksToPlay[i])->getDeltaTime() + tick;
                    i--; //Check if we have several events with delta time of 0
                }
            }
            //std::cout << tick << std::endl;
            startTime += std::chrono::nanoseconds(static_cast<int>(curSongStat_.nsTempo));
            curTime = std::chrono::high_resolution_clock::now();
            if(std::chrono::nanoseconds(static_cast<int>(curSongStat_.nsTempo)) 
            <= std::chrono::duration_cast<std::chrono::nanoseconds>(curTime - startTime))
            {
                std::cerr << "ERROR: ticks are late!" << std::endl;
            }
            tick++;
        }
    }



    /*while(it != trackEvents.end() && running)
    {
        curTime = std::chrono::high_resolution_clock::now();
        if(std::chrono::nanoseconds((int)(curSongStat_.nsTempo * 1000)) <= std::chrono::duration_cast<std::chrono::nanoseconds>(curTime - startTime))
        {
            if((*it)->getDeltaTime() <= (ticks - ticksLastEvent))
            {
                (*it)->execute(usbCom, this);
                it++;
                ticksLastEvent = ticks;
            }
            startTime = curTime;
            ticks++;
            if(0 != usbCom.readData(buff, 100))//TODO debug
            {
                printf("%s\n", buff);
            }
            std::cout << ticks << std::endl;
        }
    }*/
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