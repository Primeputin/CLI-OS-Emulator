#pragma once
#include "Console.h"
#include <string>
#include <vector>
#include <sstream>

using namespace std;

class MainConsole : public Console
{
    public:
        static MainConsole& getInstance();

        void printCSOPESYBanner();

        void recognizedCommand(string command);

        void initialize(string command);

        void screen(string command);

        void schedulerStart(string command);

        void schedulerStop(string command);

        void reportUtil(string command);

        virtual int processCommand(string command) override; 

        virtual int getCommand() override;

};

