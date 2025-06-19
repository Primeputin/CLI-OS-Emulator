#pragma once

#include "Process.h"
#include "Console.h"
#include "Scheduler.h"
#include "PrintCommand.h"
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

		shared_ptr<Scheduler> scheduler;
	public:
		bool running = false;
		shared_ptr<Console> currentConsole;
		static ConsoleManager* getInstance();
		static void initialize();
		static void destroy();
		void createProcess(string name);
		void destroyProcess(string name);
		void switchToMain();
		void switchToProcessConsole(string name);
		void addToConsoleTable(string name, shared_ptr<Console> console);
		bool consoleExists(string name) const;
		void stop();

		void readConfigFile();
		void initScheduler();
		void runScheduler();
		void stopScheduler();
		void listProcesses(bool outToFile);

		bool isSchedulerInitialized();
		void generateProcesses();
		void stopGenerationOfProcesses();
};

