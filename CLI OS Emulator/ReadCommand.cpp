#include "ReadCommand.h"
#include "Process.h"
#include <iostream>

ReadCommand::ReadCommand(int pid, const std::string varName, std::string address, Process* process)
	: ICommand(pid, CommandType::READ), varName(varName), address(address) {
	setProcess(process); // Set the process for this command
}

void ReadCommand::execute() {
	if (process != nullptr) {

		if (address.substr(0, 2) == "0x") 
		{
			string hex = address.substr(2, 4);

			uint16_t addressValue = std::stoi(hex, nullptr, 16);
			process->readVariable(varName, addressValue); // Read the variable from the process's memory

			
		}
		else
		{
			process->readVariable(varName, address); 
		}

		
	}
}


void ReadCommand::logExecute(int cpuCoreID, std::string fileName) {

	execute();
}