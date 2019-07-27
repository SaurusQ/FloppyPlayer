#ifndef SERIAL_HPP
#define SERIAL_HPP

#define ARDUINO_WAIT_TIME 2000

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

class Serial
{
    public:
        //Initialize Serial communication with the given COM port
        Serial(const char *portName, unsigned int baudRate);
        //Close the connection
        ~Serial();
        //Read data in a buffer, if nbChar is greater than the
        //maximum number of bytes available, it will return only the
        //bytes available. The function return -1 when nothing could
        //be read, the number of bytes actually read.
        int readData(uint8_t *buffer, unsigned int bytes);
        //Writes data from a buffer through the Serial connection
        //return true on success.
        bool writeData(const uint8_t *buffer, unsigned int bytes);
        bool isConnected();

    private:
        //Serial comm handler
        HANDLE hSerial_;
        //Connection status
        bool connected_;
        //Get various information about the connection
        COMSTAT status_;
        //Keep track of last error
        DWORD errors_;


};

#endif
