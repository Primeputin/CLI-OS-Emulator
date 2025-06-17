#include "DeclareCommand.h"

DeclareCommand::DeclareCommand(int pid, string& varName, uint16_t value, Process* process) : ICommand(pid, CommandType::DECLARE), varName(std::move(varName)), value(value)
{
    this->process = process;  
}

void DeclareCommand::execute() {
    if (process) {
        process->declareVariable(varName, value);
    }
}

void DeclareCommand::logExecute(int cpuCoreID, std::string fileName)
{
    // temporary
    if (process) {
        process->declareVariable(varName, value);
    }
}