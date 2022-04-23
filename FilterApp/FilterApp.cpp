// FilterApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "EventWatcher.h"
#include <conio.h>

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "");
    std::cout << "Start Event Listening" << std::endl;
    std::string s;
    if (argc > 1) 
    {
        s = argv[1];
    }
    else 
    {
        std::cout << "No path been input." << std::endl;
        s = "C:\\Users\\user\\Documents\\watch\\*";
    }

    EventWatcher ew(FuncTool::convertPathToVolume(s));
    ew.connectAndWatch();

    
}


