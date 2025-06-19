#include "PrintVariableCommand.h"
#include "Process.h" 
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>

PrintVariableCommand::PrintVariableCommand(int pid, std::string varName, Process* process)
    : ICommand(pid, ICommand::PRINT), varName(varName) {
    this->process = process;
}

void PrintVariableCommand::execute() {
    uint16_t value;
    process->getVariableValue(varName, value);
    std::cout << value << " from: " << varName << std::endl;
}

void PrintVariableCommand::logExecute(int cpuCoreID, std::string fileName) {
    uint16_t value;
    process->getVariableValue(this->varName, value);

    time_t now = time(0);
    tm localTime;
    localtime_s(&localTime, &now);

    std::ostringstream oss;
    oss << "(" << std::put_time(&localTime, "%m/%d/%Y %I:%M:%S%p") << ")"
        << " Core:" << std::setw(2) << cpuCoreID
        << "  " << value << " from: " << this->varName;

    process->addLog(oss.str());
}
