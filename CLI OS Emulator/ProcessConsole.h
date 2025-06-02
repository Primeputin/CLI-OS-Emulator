#pragma once

#include "Process.h"
#include "Console.h"

#include <iostream>
#include <string>

using namespace std;

class ProcessConsole : public Console
{
	public:
		ProcessConsole(std::shared_ptr<Process> process);

		void header() override;

		virtual void processCommand(string command) override;

		virtual void getCommand() override;
	private:
		shared_ptr<Process> process;
};

