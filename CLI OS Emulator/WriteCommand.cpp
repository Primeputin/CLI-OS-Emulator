
#include "WriteCommand.h"
#include "Process.h"
#include <iostream>

WriteCommand::WriteCommand(int pid, std::string address, uint16_t value, Process* process)
	: ICommand(pid, CommandType::WRITE), address(address), value(value) {
	setProcess(process); // Set the process for this command
}

void WriteCommand::execute() {
	if (process != nullptr) {
		if (address.substr(0, 2) == "0x")
		{
			std::string hex = address.substr(2, 4);
			uint16_t addressValue = std::stoi(hex, nullptr, 16);
			// TODO: check if the address parameter is a literal address or variable name
			process->writeToMemory(addressValue, value); 
		}
		else
		{
			process->writeToMemory(address, value); 
		}
	}
}

void WriteCommand::logExecute(int cpuCoreID, std::string fileName) {

	execute();
}
