#pragma once

#include "Process.h"
#include "Console.h"

#include <iostream>
#include <string>

using namespace std;

class ProcessConsole : public Console
{
	public:

		void header(Process process) override;

		virtual void processCommand(string command) override;

		virtual void getCommand() override;
};

