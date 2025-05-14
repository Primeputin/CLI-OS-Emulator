#include "ProcessConsole.h"
#include "Console.h"
#include <iostream>

ProcessConsole& ProcessConsole::getInstance() {
	static ProcessConsole instance;
	return instance;
}

void ProcessConsole::displayProcessInfo(Process process)
{
	cout << "Process Name: " << process.getName() << endl;
	cout << "Current Line: " << process.getCurrentLine() << endl;
	cout << "Total Lines: " << process.getTotalLines() << endl;
}

int ProcessConsole::processCommand(string command)
{
	vector<string> texts = getSpacedTexts(command);
	if (command == "exit")
	{
		Console::clear();
		ConsoleManager::getInstance().switchToMain();
		return 1;
	}
	else if (!texts.empty())
	{
		printOutInvalidCommand(command);
	}
	return 0;
}

int ProcessConsole::getCommand()
{
	string command;
	cout << "Enter a command: ";
	getline(cin, command);
	return processCommand(command);
}

