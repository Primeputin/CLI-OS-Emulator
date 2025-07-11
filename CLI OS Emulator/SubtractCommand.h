#pragma once

#include <string>
#include "ICommand.h"
#include "Process.h"

class SubtractCommand : public ICommand
{
    public:
        SubtractCommand(int pid, const std::string& targetVar, const std::string& op1, const std::string& op2, Process* process);

        void execute() override;
        void logExecute(int cpuCoreID, std::string fileName) override;

    private:
        std::string targetVar;
        std::string operand1;
        std::string operand2;
};

