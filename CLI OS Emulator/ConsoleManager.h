#pragma once

#include "Process.h"

#include <string>
#include <unordered_map>

using namespace std;

class ConsoleManager
{
	private:
		unordered_map<std::string, Process> processTable;
	public:
		static ConsoleManager& getInstance();
		void start();
		void createProcess(string name);
		void switchToMain();
		void switchToProcessConsole(string name);
};

