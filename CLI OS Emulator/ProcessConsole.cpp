#include "ConsoleManager.h"
#include "ProcessConsole.h"
#include "Console.h"
#include <iostream>

void ProcessConsole::header(Process process)
{
	cout << "Process Name: " << process.getName() << endl;
	cout << "Current Line: " << process.getCurrentLine() << endl;
	cout << "Total Lines: " << process.getTotalLines() << endl;
	cout << "Date and time Created: ";
	show_dateTime(process.getCreatedTime());
}

void ProcessConsole::processCommand(string command)
{
	vector<string> texts = getSpacedTexts(command);
	if (command == "exit")
	{
		Console::clear();
		ConsoleManager::getInstance().switchToMain();
	}
	else if (!texts.empty())
	{
		printOutInvalidCommand(command);
	}
}

void ProcessConsole::getCommand()
{
	string command;
	cout << "Enter a command: ";
	getline(cin, command);
	processCommand(command);
}

