#include "ConsoleManager.h"
#include "MainConsole.h"
#include "ProcessConsole.h"
#include "Console.h"
#include <memory>

ConsoleManager& ConsoleManager::getInstance()
{
	static ConsoleManager instance;
	return instance;
}

void ConsoleManager::run() {
	this->running = true; // Set running to true when starting the console manager
	this->currentConsole = make_unique<MainConsole>(); // Initialize the current console to MainConsole
	this->currentConsole->header(); // Call the header function of the current console
	while (this->running)
	{

		this->currentConsole->getCommand();
	}

}

void ConsoleManager::createProcess(string name)
{
	if (processTable.find(name) != this->processTable.end()) {
		std::cerr << "Screen name " << name << " already exists. Please use a different name." << std::endl;
		return;
	}

	processTable.insert({ name, Process(name, 0, 50) }); // temporary

	cout << "Screen name: " << name << " created successfully." << std::endl;

	switchToProcessConsole(name);
}

void ConsoleManager::switchToMain()
{
	Console::clear();
	this->currentConsole = make_unique<MainConsole>();
	this->currentConsole->header();
}

void ConsoleManager::switchToProcessConsole(string name)
{
	if (processTable.contains(name)) {
		Console::clear();
		this->currentConsole = make_unique<ProcessConsole>();
		this->currentConsole->show_dateTime();
		this->currentConsole->header(processTable.at(name));
	}
	else {
		cerr << "None found" << endl;
	}
}

void ConsoleManager::stop()
{
	this->running = false; // Set running to false to stop the console manager
}

