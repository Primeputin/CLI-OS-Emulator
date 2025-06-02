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
		unordered_map<string, shared_ptr<Console>> consoleTable;
		ConsoleManager();
		~ConsoleManager() = default;
		ConsoleManager(ConsoleManager const&) {}; // copy constructor is private
		ConsoleManager& operator=(ConsoleManager const&) {}; // assignment operator is private
		static ConsoleManager* sharedInstance;
	public:
		bool running = false;
		shared_ptr<Console> currentConsole;
		static ConsoleManager* getInstance();
		static void initialize();
		static void destroy();
		void createProcess(string name);
		void switchToMain();
		void switchToProcessConsole(string name);
		void stop();
};

