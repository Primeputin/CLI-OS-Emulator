#include "ConsoleManager.h"
#include "MainConsole.h"
#include "ProcessConsole.h"

ConsoleManager& ConsoleManager::getInstance()
{
	static ConsoleManager instance;
	return instance;
}

void ConsoleManager::start() {
	MainConsole& mainConsole = MainConsole::getInstance();
	ProcessConsole& processConsole = ProcessConsole::getInstance();

	mainConsole.setIsOnDisplay(true);
	processConsole.setIsOnDisplay(false);
	mainConsole.printCSOPESYBanner();
	bool exit = false;
	while (!exit)
	{

		if (mainConsole.getIsOnDisplay()) {
			exit = mainConsole.getCommand();
		}
		else if (processConsole.getIsOnDisplay()) 
		{
			processConsole.getCommand();
		}
		else {
			std::cerr << "No console is currently displayed." << std::endl;
			exit = true; // Exit the loop if no console is displayed
		}
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
	MainConsole& mainConsole = MainConsole::getInstance();
	ProcessConsole& processConsole = ProcessConsole::getInstance();
	Console::clear();
	mainConsole.printCSOPESYBanner();
	mainConsole.setIsOnDisplay(true);
	processConsole.setIsOnDisplay(false);
}

void ConsoleManager::switchToProcessConsole(string name)
{
	MainConsole& mainConsole = MainConsole::getInstance();
	ProcessConsole& processConsole = ProcessConsole::getInstance();
	if (processTable.contains(name)) {
		Console::clear();
		processConsole.show_dateTime();
		processConsole.displayProcessInfo(processTable.at(name));
		mainConsole.setIsOnDisplay(false);
		processConsole.setIsOnDisplay(true);
	}
	else {
		cerr << "None found" << endl;
	}
}

