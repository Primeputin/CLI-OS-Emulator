#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <iostream>
#include "ConsoleManager.h"

using namespace std;

class Console
{
	public:
		static void clear();
	//protected:

		bool isOnDisplay;


		void printOutInvalidCommand(string command);

		vector<string> getSpacedTexts(string command);

		void show_dateTime();

		virtual int processCommand(string command) = 0; // must be overridden/implemented in derived classes

		virtual int getCommand() = 0;

    //public:

		bool getIsOnDisplay() const;

		void setIsOnDisplay(bool isOnDisplay);
};

