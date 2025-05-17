#pragma once
#include "Console.h"
#include <string>
#include <vector>
#include <sstream>

using namespace std;

class MainConsole : public Console
{
    public:
		MainConsole() = default; // Default constructor

        void header() override;

        void recognizedCommand(string command);

        void initialize(string command);

        void screen(string command);

        void schedulerStart(string command);

        void schedulerStop(string command);

        void reportUtil(string command);

        virtual void processCommand(string command) override; 

        virtual void getCommand() override;

};

