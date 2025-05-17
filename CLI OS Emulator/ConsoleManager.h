#pragma once

#include "Process.h"
#include "Console.h"
#include <memory>
#include <string>
#include <unordered_map>

using namespace std;

class ConsoleManager
{
	private:
		unordered_map<std::string, Process> processTable;
		bool running = false;
		unique_ptr<Console> currentConsole;
	public:
		static ConsoleManager& getInstance();
		void run();
		void createProcess(string name);
		void switchToMain();
		void switchToProcessConsole(string name);
		void stop();
};

