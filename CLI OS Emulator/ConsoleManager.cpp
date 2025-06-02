#include "ConsoleManager.h"
#include "MainConsole.h"
#include "ProcessConsole.h"
#include "Console.h"
#include <memory>

const string MAIN_CONSOLE_NAME = "MAIN_CONSOLE";
const string PROCESS_SCREEN_PREFIX = "PROCESS_SCREEN_";

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
	if (consoleTable.find(PROCESS_SCREEN_PREFIX + name) != this->consoleTable.end()) {
		std::cerr << "Screen name " << name << " already exists. Please use a different name." << std::endl;
		return;
	}

	shared_ptr<Process> process = make_shared<Process>(name, 50, 0);

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

void ConsoleManager::stop()
{
	this->running = false; // Set running to false to stop the console manager
}

