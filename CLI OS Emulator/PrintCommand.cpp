#include "PrintCommand.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include "Process.h" 

PrintCommand::PrintCommand(int pid, std::string toPrint, Process* process)
    : ICommand(pid, ICommand::PRINT), toPrint(toPrint) {
    this->process = process;
}

void PrintCommand::execute() {
    std::cout << this->toPrint << std::endl;
}

void PrintCommand::logExecute(int cpuCoreID, std::string fileName) {
    time_t now = time(0);
    tm localTime;
    localtime_s(&localTime, &now);

    std::ostringstream oss;
    oss << "(" << std::put_time(&localTime, "%m/%d/%Y %I:%M:%S%p") << ")"
        << " Core:" << std::setw(2) << cpuCoreID
        << "  \"" << this->toPrint << "\"";

    process->addLog(oss.str());
}