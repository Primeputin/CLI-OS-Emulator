#include "ProcessConsole.h"
#include "ConsoleManager.h"
#include "Process.h" 
#include <iostream>
#include <iomanip>
#include <sstream>

ProcessConsole::ProcessConsole(std::shared_ptr<Process> newProcess)
    : Console(newProcess->getName()), process(newProcess) {
}

void ProcessConsole::header() {
    std::cout << "Process Name: " << process->getName() << std::endl;
    std::cout << "Current Line: " << process->getCurrentLine() << std::endl;
    std::cout << "Total Lines: " << process->getTotalLines() << std::endl;
    std::cout << "Date and time Created: ";
    show_dateTime(process->getCreatedTime());
}

void ProcessConsole::processCommand(std::string command) {
    std::vector<std::string> texts = getSpacedTexts(command);

    if (command == "exit") {
        Console::clear();
        ConsoleManager::getInstance()->switchToMain();
    }
    else if (texts[0] == "process-smi") {

        std::cout << "Process name: " << process->getName() << std::endl;
        std::cout << "ID: " << process->getPID() << std::endl;
        std::cout << "Logs:" << std::endl;

        for (const auto& log : process->getLogs()) {
            std::cout << log << std::endl;
        }

        if (process->isFinished()) {
            std::cout << "\nFinished!" << std::endl;
        }
        else {
            std::cout << "\nCurrent instruction line: " << process->getCurrentLine() << std::endl;
            std::cout << "Lines of code: " << process->getTotalLines() << std::endl;
        }
    }
    else if (!texts.empty()) {
        printOutInvalidCommand(command);
    }
}

void ProcessConsole::getCommand() {
    std::string command;
    std::cout << "Enter a command: ";
    std::getline(std::cin, command);
    processCommand(command);
}
