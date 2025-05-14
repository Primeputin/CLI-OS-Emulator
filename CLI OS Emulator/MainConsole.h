#pragma once
#include <string>
#include <vector>
#include <sstream>

using namespace std;

class MainConsole
{
    public:
        static MainConsole& getInstance();

		void run();


    private:
        MainConsole() = default;  // Private constructor
        MainConsole(const MainConsole&) = delete;             // No copying
        MainConsole& operator=(const MainConsole&) = delete;  // No assignment

        void printCSOPESYBanner();

        void recognizedCommand(string command);

        void initialize(string command);

        void screen(string command);

        void schedulerStart(string command);

        void schedulerStop(string command);

        void reportUtil(string command);

        void clear();

        vector<string> getSpacedTexts(string command);

        void processCommand(string command);

        void getCommand();

};

