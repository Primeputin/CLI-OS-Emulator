
#include "WriteCommand.h"
#include "Process.h"
#include <iostream>
#include <algorithm> 

WriteCommand::WriteCommand(int pid, std::string address, string valueToBeWritten, Process* process)
	: ICommand(pid, CommandType::WRITE), address(address), valueToBeWritten(valueToBeWritten) {
	setProcess(process); // Set the process for this command
}

void WriteCommand::execute() {
	if (process != nullptr) {

		uint16_t actualValue = 0;

		auto resolve = [this](const std::string& op) -> uint16_t {
			if (std::all_of(op.begin(), op.end(), ::isdigit)) {
				return static_cast<uint16_t>(std::stoi(op)); // returns the value of the actual operand if it's a number
			}
			uint16_t value = 0;
			process->getVariableValue(op, value); // Defaults to 0 if undeclared
			return value;
			};

		actualValue = resolve(valueToBeWritten);


		if (address.substr(0, 2) == "0x")
		{
			std::string hex = address.substr(2, 4);
			uint16_t addressValue = std::stoi(hex, nullptr, 16);

			if (addressValue >= process->getMemorySize() || addressValue < 0) {
				process->setProcessState(Process::SHUTDOWNED);
				process->setShutdownAddress(hex);
				process->setShutdownTime();
				return; // Exit if the address is out of bounds
			}

			process->writeToMemory(addressValue, actualValue);
		}
		else
		{
			process->writeToMemory(address, actualValue);
		}
	}
}

void WriteCommand::logExecute(int cpuCoreID, std::string fileName) {

	execute();
}
