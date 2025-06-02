#include "ConsoleManager.h"
#include "ProcessConsole.h"
#include "Console.h"
#include <iostream>


ProcessConsole::ProcessConsole(std::shared_ptr<Process> newProcess)
	: Console("PROCESS_SCREEN_" + newProcess->getName()), process(newProcess) {
}

void ProcessConsole::header()
{
	cout << "Process Name: " << this->process->getName() << endl;
	cout << "Current Line: " << this->process->getCurrentLine() << endl;
	cout << "Total Lines: " << this->process->getTotalLines() << endl;
	cout << "Date and time Created: ";
	show_dateTime(this->process->getCreatedTime());
}

void ProcessConsole::processCommand(string command)
{
	vector<string> texts = getSpacedTexts(command);
	if (command == "exit")
	{
		Console::clear();
		ConsoleManager::getInstance()->switchToMain();
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

