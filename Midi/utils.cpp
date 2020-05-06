
#include "utils.hpp"

#include "midiData.hpp"

uint32_t my_ntohl(uint32_t a)
{
    return ((a << 24) & 0xFF000000) | ((a << 8) & 0x00FF0000) | ((a >> 24) & 0x000000FF) | ((a >> 8) & 0x0000FF00);
}

uint16_t my_ntohs(uint16_t a)
{
    return ((a << 8) & 0xFF00) | ((a >> 8) & 0x00FF);
}

uint32_t readVarLenArg(std::ifstream &midiFile, int64_t &bytesLeft)
{
    //Read <delta-time>
    uint8_t deltaArr[DELTA_MAX_LEN + 1];
    unsigned int deltaLen = 0;
    memset(deltaArr, 0, DELTA_MAX_LEN + 1);
    while(1)
    {
        if(deltaLen > DELTA_MAX_LEN)
        {
            std::cerr << "ERROR: Event in position " << midiFile.tellg() << " corrupted." << std::endl;
            return (uint32_t)-1;
        }
        deltaArr[deltaLen] = midiFile.get();
        bytesLeft--;
        if(0 == (deltaArr[deltaLen] & 0x80))
        {
            //Last bit is 0 so it's the last byte of <delta-time>
            deltaLen++;
            break;
        }
        deltaLen++;
    }
    
    //Reconstruct <delta-time>
    //TODO doesn't use ntohl may fail in big endian machines
    uint32_t deltaTime = 0;
    deltaTime = (deltaArr[0] & 0x7F) << (3 * 8);
    for(unsigned int i = 1; i < deltaLen; i++)
    {
        deltaTime = (deltaTime >> 1) | ((deltaArr[i] & 0x7F) << ((3 - i) * 8));
    }
    deltaTime = deltaTime >> ((DELTA_MAX_LEN - deltaLen) * 8);
    return deltaTime;
}