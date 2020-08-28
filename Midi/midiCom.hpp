
#ifndef MIDI_COM_H
#define MIDI_COM_H

//Event types
enum EventType
{
    E_IGNORE =          0x00,
    E_NOTE =            0x01,
    E_STOP_NOTE =       0x02,
    E_PLAY_CONNECTED =  0x03,
    E_END_OF_TRACK =    0x08,
};
//Other control information send over USB
#define RESET_ALL   0xFF

#endif
