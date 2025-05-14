#pragma once

#include "Process.h"
#include "Console.h"

#include <iostream>
#include <string>

using namespace std;

class ProcessConsole : public Console
{
	public:
		static ProcessConsole& getInstance();

		void displayProcessInfo(Process process);

		virtual int processCommand(string command) override;

		virtual int getCommand() override;
};

