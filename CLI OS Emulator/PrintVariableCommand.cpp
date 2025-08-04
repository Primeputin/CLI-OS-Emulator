#include "PrintVariableCommand.h"
#include "Process.h" 
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>

PrintVariableCommand::PrintVariableCommand(int pid, string text, std::string varName, Process* process)
    : ICommand(pid, ICommand::PRINT), text(text), varName(varName) {
    this->process = process;
}

void PrintVariableCommand::execute() {
    uint16_t value;

    process->getVariableValue(varName, value);

    if (text == "") {   
        cout << value << std::endl;
    }
    else {
		cout << text << value << endl;
    }

    
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
        << "  " << text <<  value << endl;

    process->addLog(oss.str());
}
