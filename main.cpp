
#include <string.h>
#include <iostream>
#include <iomanip>
#include <windows.h>

#include "midiPlayer.hpp"

//Handling console(cursor)
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
COORD CursorPosition;
void setCursor(int x, int y)
{
    CursorPosition.X = x;
    CursorPosition.Y = y;
    SetConsoleCursorPosition(console, CursorPosition);
}

int main()
{
    //The Pink Panther Theme.mid
    MidiPlayer test("S:\\Music\\Floppy Songs Audacity\\"
    //"Fallout - Mission; Imposible - Fallout (Piano Cover) - MIDI"
    //"DOOM (2016) - BFG Division"
    //"Doom - Level 9"
    //"Initial D - Running in the 90s"
    //"Star Wars The Force Awakens OST - March of The Resistance"//Real time category
    //"Undertale - Its Raining Somewhere Else" //Instrumental data
    //"Your_Name_-_Tate_no_Yuusha_no_Nariagari_ED"//Second track empty????
    //"Konosuba op fantastic dreamer"
    //"Pac-Man World - Ghostly Garden"
    //"Dragon Ball Super - Ultimate Battle"
    //"Full Metal Alchemist - Lapis Philosophorum"
    //"Pirates of the Caribbean - He's a Pirate (3)"
    //"Stardew Valley - Overture" //Nice
    //"Avengers- Age of Ultron - Heroes"
    //"Imagine Dragons - Believer"
    //"Dark Souls II - Menu Theme"
    //"Disney_Themes_-_DuckTales_2017_Full"
    //"Another_Colony_Piano_Version"
    "Nintendo_-_Pokemon_Fire_Red_Route_1_Piano_Cover_Hard_Version"
    //"Mega Man 2 - Quickman"
    //"Two Faced Lovers 19.4 Million"
    //"RISE_tvsize"
    //"The Pink Panther Theme"//Not working
    //"Rightfully"
    //"test"
    ".mid");
    test.parse();
    test.printData();

    test.configurePlay();
    test.playUSB();
    


    while(1);
    return 0;
}
