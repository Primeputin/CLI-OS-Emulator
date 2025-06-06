#include "ConsoleManager.h"
#include "MainConsole.h"
#include "ProcessConsole.h"
#include "Console.h"
#include <memory>

const string MAIN_CONSOLE_NAME = "MAIN_CONSOLE";

ConsoleManager* ConsoleManager::sharedInstance = nullptr;
ConsoleManager* ConsoleManager::getInstance()
{
	return sharedInstance;
}

void ConsoleManager::initialize()
{
	if (sharedInstance) {
		std::cerr << "ConsoleManager is already initialized." << std::endl;
		return;
	}
	sharedInstance = new ConsoleManager();
}

void ConsoleManager::destroy()
{
	if (sharedInstance) {
		delete sharedInstance;
		sharedInstance = nullptr;
	}
}

ConsoleManager::ConsoleManager() {
	this->running = true; // Set running to true when starting the console manager
	shared_ptr<MainConsole> mainConsole = make_shared<MainConsole>();
	consoleTable.insert({ mainConsole->getName(), mainConsole });
	this->currentConsole = mainConsole;
	this->currentConsole->header(); // Call the header function of the current console
	switchToMain(); // Switch to the main console
}

void ConsoleManager::createProcess(string name)
{
	if (consoleTable.find(name) != this->consoleTable.end()) {
		std::cerr << "Screen name " << name << " already exists. Please use a different name." << std::endl;
		return;
	}

	// <-- Temporary -->
	vector<shared_ptr<ICommand>> commandList;
	for (int j = 0; j < 100; j++)
	{
		commandList.push_back(make_shared<PrintCommand>(100, "Hello World!"));
	}

	shared_ptr<Process> process = make_shared<Process>(100, name, commandList);

	// <-- Temporary -->

	shared_ptr<ProcessConsole> newConsole = make_shared<ProcessConsole>(process);

	consoleTable.insert({ newConsole->getName(), newConsole}); // temporary

	cout << "Screen name: " << name << " created successfully." << std::endl;

	switchToProcessConsole(newConsole->getName());
}

void ConsoleManager::switchToMain()
{
	Console::clear();
	this->currentConsole = this->consoleTable.at(MAIN_CONSOLE_NAME);
	this->currentConsole->header();
}

void ConsoleManager::switchToProcessConsole(string name)
{
	if (consoleTable.contains(name)) {
		Console::clear();
		this->currentConsole = this->consoleTable.at(name);
		this->currentConsole->header();
	}
	else {
		cerr << "None found" << endl;
	}
}

void ConsoleManager::addToConsoleTable(string name, shared_ptr<Console> console)
{
	if (consoleTable.find(name) == consoleTable.end()) {
		consoleTable.insert({ name, console });
	}
	else {
		cerr << "Console with name " << name << " already exists." << endl;
	}
}

void ConsoleManager::stop()
{
	this->running = false; // Set running to false to stop the console manager
}

void ConsoleManager::initScheduler()
{
	if (!scheduler) {
		scheduler = make_shared<Scheduler>(Scheduler::FCFS, 4, 0, 0, 0, 0, 33);
	}
	else {
		cerr << "Scheduler is already initialized." << endl;
	}
}

void ConsoleManager::runScheduler()
{
	if (scheduler) {
		scheduler->run();
	}
	else {
		cerr << "Scheduler is not initialized." << endl;
	}
}

void ConsoleManager::listProcesses()
{
	if (scheduler) {
		scheduler->printProcessesStatus();
	}
	else {
		cerr << "Scheduler is not initialized." << endl;
	}
}



