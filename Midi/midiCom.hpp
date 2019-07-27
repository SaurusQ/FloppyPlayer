
#ifndef MIDI_COM_H
#define MIDI_COM_H

//Event types (send also over USB)
enum EventType
{
    E_IGNORE =          0x00,
    E_NOTE =            0x01,
    E_NOTECTRL =        0x02,
    E_META =            0x03,
    E_SYSEX =           0x04,
    E_TEMPO =           0x05,
    E_TIME_SIGN =       0x06,
    E_KEY_SIGN =        0x07,
    E_END_OF_TRACK =    0x08
};
//Other control information send over USB
#define RESET_ALL       0xFF

#endif
