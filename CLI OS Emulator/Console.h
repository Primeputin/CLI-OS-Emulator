#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <iostream>
#include "Process.h"

using namespace std;

class Console
{
	public:
		
		Console(string name);

		virtual ~Console() = default;

		static void clear();

		void printOutInvalidCommand(string command);

		vector<string> getSpacedTexts(string command);

		void show_dateTime(time_t time);

		virtual void header(); // optional to be overridden

		virtual void processCommand(string command) = 0; // must be overridden/implemented in derived classes

		virtual void getCommand() = 0;

		const string& getName() const;


	protected:
		string name;

};

