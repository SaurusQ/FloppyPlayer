
#include "Serial.hpp"

Serial::Serial(const char *portName, unsigned int baudRate)
{
    //We're not yet connected
    this->connected_ = false;

    //Try to connect to the given port throuh CreateFile
    this->hSerial_ = CreateFile(portName,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

    //Check if the connection was successfull
    if(this->hSerial_ == INVALID_HANDLE_VALUE)
    {
        //If not success full display an Error
        if(GetLastError() == ERROR_FILE_NOT_FOUND){
            std::cerr << "ERROR: Handle was not attached. Reason: " << portName << " not available." << std::endl;

        }
        else
        {
            std::cerr << "ERROR: N/A" << std::endl;
        }
    }
    else
    {
        //If connected we try to set the comm parameters
        DCB dcbSerialParams = {0};

        //Try to get the current
        if (!GetCommState(this->hSerial_, &dcbSerialParams))
        {
            std::cerr << "ERROR: failed to get current serial parameters!" << std::endl;
        }
        else
        {
            //Define serial connection parameters for the arduino board
            dcbSerialParams.BaudRate = baudRate;
            dcbSerialParams.ByteSize = 8;
            dcbSerialParams.StopBits = ONESTOPBIT;
            dcbSerialParams.Parity   = NOPARITY;
            //Setting the DTR to Control_Enable ensures that the Arduino is properly
            //reset upon establishing a connection
            dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

             //Set the parameters and check for their proper application
             if(!SetCommState(hSerial_, &dcbSerialParams))
             {
                std::cerr << "ERROR: Could not set Serial Port parameters" << std::endl;
             }
             else
             {
                 //If everything went fine we're connected
                 this->connected_ = true;
                 //Flush any remaining characters in the buffers 
                 PurgeComm(this->hSerial_, PURGE_RXCLEAR | PURGE_TXCLEAR);
                 //We wait 2s as the arduino board will be reseting
                 Sleep(ARDUINO_WAIT_TIME);
             }
        }
    }

}

Serial::~Serial()
{
    //Check if we are connected before trying to disconnect
    if(this->connected_)
    {
        //We're no longer connected
        this->connected_ = false;
        //Close the serial handler
        CloseHandle(this->hSerial_);
    }
}

int Serial::readData(uint8_t *buffer, unsigned int bytes)
{
    //Number of bytes we'll have read
    DWORD bytesRead;
    //Number of bytes we'll really ask to read
    unsigned int toRead;

    //Use the ClearCommError function to get status info on the Serial port
    ClearCommError(this->hSerial_, &this->errors_, &this->status_);

    //Check if there is something to read
    if(this->status_.cbInQue > 0)
    {
        //If there is we check if there is enough data to read the required number
        //of characters, if not we'll read only the available characters to prevent
        //locking of the application.
        if(this->status_.cbInQue > bytes)
        {
            toRead = bytes;
        }
        else
        {
            toRead = this->status_.cbInQue;
        }

        //Try to read the require number of chars, and return the number of read bytes on success
        if(ReadFile(this->hSerial_, buffer, toRead, &bytesRead, NULL) )
        {
            return bytesRead;
        }

    }

    //If nothing has been read, or that an error was detected return 0
    return 0;

}


bool Serial::writeData(const uint8_t *buffer, unsigned int bytes)
{
    DWORD bytesSend;

    //Try to write the buffer on the Serial port
    if(!WriteFile(this->hSerial_, (void *)buffer, bytes, &bytesSend, 0))
    {
        //In case it don't work get comm error and return false
        ClearCommError(this->hSerial_, &this->errors_, &this->status_);
        return false;
    }
    return true;
}

bool Serial::isConnected()
{
    return this->connected_;
}
