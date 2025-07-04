#include "SubtractCommand.h"
#include <algorithm>
#include <iostream>

SubtractCommand::SubtractCommand(int pid, const std::string& targetVar, const std::string& op1, const std::string& op2, Process* process)
    : ICommand(pid, CommandType::ADD), targetVar(targetVar), operand1(op1), operand2(op2)
{
    this->process = process;
}

void SubtractCommand::execute() {
    if (!process) return;

    uint16_t val1 = 0, val2 = 0;

    // Helper: determine if operand is numeric
    auto resolve = [this](const std::string& op) -> uint16_t {
        if (std::all_of(op.begin(), op.end(), ::isdigit)) {
            return static_cast<uint16_t>(std::stoi(op)); // returns the value of the actual operand if it's a number
        }
        uint16_t value = 0;
        process->getVariableValue(op, value); // Defaults to 0 if undeclared
        return value;
        };

    val1 = resolve(operand1);
    val2 = resolve(operand2);

    uint16_t result;

    if (val1 < val2) {
        result = 0; // Ensure result is not negative
    }
    else {
        result = static_cast<int>(val1) - static_cast<int>(val2);
    }
    process->declareVariable(targetVar, result); // Overwrites if exists
}

void SubtractCommand::logExecute(int cpuCoreID, std::string fileName) {

    if (!process) return;

    uint16_t val1 = 0, val2 = 0;

    // Helper: determine if operand is numeric
    auto resolve = [this](const std::string& op) -> uint16_t {
        if (std::all_of(op.begin(), op.end(), ::isdigit)) {
            return static_cast<uint16_t>(std::stoi(op)); // returns the value of the actual operand if it's a number
        }
        uint16_t value = 0;
        process->getVariableValue(op, value); // Defaults to 0 if undeclared
        return value;
        };

    val1 = resolve(operand1);
    val2 = resolve(operand2);


    uint16_t result;

    if (val1 < val2) {
        result = 0; // Ensure result is not negative
    }
    else {
        result = static_cast<int>(val1) - static_cast<int>(val2);
    }
    process->declareVariable(targetVar, result); // Overwrites if exists

}