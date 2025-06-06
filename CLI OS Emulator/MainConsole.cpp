#include "MainConsole.h"
#include "ProcessConsole.h"
#include "ConsoleManager.h"
#include "Console.h"
#include "Process.h"
#include <iostream>

// ANSI escape codes for color
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define RESET   "\033[0m"

MainConsole::MainConsole() : Console("MAIN_CONSOLE") {
	
}

void MainConsole::header() {

    std::cout << R"(  
  ______    ______    ______   _______   ________   ______   __      __ 
 /      \  /      \  /      \ /       \ /        | /      \ /  \    /  |
/$$$$$$  |/$$$$$$  |/$$$$$$  |$$$$$$$  |$$$$$$$$/ /$$$$$$  |$$  \  /$$/ 
$$ |  $$/ $$ \__$$/ $$ |  $$ |$$ |__$$ |$$ |__    $$ \__$$/  $$  \/$$/  
$$ |      $$      \ $$ |  $$ |$$    $$/ $$    |   $$      \   $$  $$/   
$$ |   __  $$$$$$  |$$ |  $$ |$$$$$$$/  $$$$$/     $$$$$$  |   $$$$/    
$$ \__/  |/  \__$$ |$$ \__$$ |$$ |      $$ |_____ /  \__$$ |    $$ |    
$$    $$/ $$    $$/ $$    $$/ $$ |      $$       |$$    $$/     $$ |    
 $$$$$$/   $$$$$$/   $$$$$$/  $$/       $$$$$$$$/  $$$$$$/      $$/     )" << '\n';


    std::cout << '\n';

    std::cout << GREEN << "Hello, Welcome to CSOPESY commandline!" << RESET << '\n';
    std::cout << YELLOW << "Type 'exit' to quit, 'clear' to clear the screen" << RESET << '\n';
    std::cout << '\n';
}

void MainConsole::recognizedCommand(string command)
{
    cout << command << " recognized, Doing something." << endl;
}

void MainConsole::initialize(string command)
{
    recognizedCommand(command);
	ConsoleManager::getInstance()->initScheduler();
    schedulerInitialized = true;
}

void MainConsole::screen(string command)
{
    recognizedCommand(command);
}

void MainConsole::schedulerStart(string command)
{
    recognizedCommand(command);
	ConsoleManager::getInstance()->runScheduler();
}

void MainConsole::schedulerStop(string command)
{
    recognizedCommand(command);
}

void MainConsole::reportUtil(string command)
{
    recognizedCommand(command);
}

void MainConsole::processCommand (string command)
{
    vector<string> texts = getSpacedTexts(command);
    if (!schedulerInitialized)
    {
        if (command == "initialize")
        {
            initialize(command);
        }
        else
        {
			cout << "You entered: " << command << "\n" << "Command invalid, please initialize the scheduler first." << "\n\n";
        }
    }
    else if (texts.size() == 1)
    {
        if (command == "initialize")
        {
            initialize(command);
        }
        else if (command == "screen")
        {
            screen(command);
        }
        else if (command == "scheduler-start")
        {
            schedulerStart(command);
        }
        else if (command == "scheduler-stop")
        {
            schedulerStop(command);
        }
        else if (command == "report-util")
        {
            reportUtil(command);
        }
        else if (command == "exit")
        {
            recognizedCommand(command);
			ConsoleManager::getInstance()->running = false;
        }
        else if (command == "clear") {
            recognizedCommand(command);
            Console::clear();
            header();
        }
        else
        {
            cout << "You entered: " << command << "\n" << "Command invalid" << "\n\n";
        }
    }
	else if (texts.size() == 2 && texts[0] == "screen" && texts[1] == "-ls")
	{
        recognizedCommand(command);
        ConsoleManager::getInstance()->listProcesses();
	}
	else if (texts.size() == 3 && texts[0] == "screen")
	{
		if (texts[1] == "-s")
		{
			recognizedCommand(command);
			ConsoleManager::getInstance()->createProcess(texts[2]);
		}
		else if (texts[1] == "-r")
		{
			recognizedCommand(command);
            ConsoleManager::getInstance()->switchToProcessConsole(texts[2]);
		}
        else
        {
            cout << "You entered: " << command << "\n" << "Command invalid" << "\n\n";
        }
	}
    else if (texts.size() != 0){
        cout << "You entered: " << command << "\n" << "Command invalid" << "\n\n";
    }
}

void MainConsole::getCommand()
{
    string command;
    cout << "Enter a command: ";
    getline(cin, command);
    processCommand(command);
}
