#include "ICommand.h"
#include "Process.h" 

ICommand::ICommand(int pid, CommandType commandType)
{
	this->pid = pid;
	this->commandType = commandType;
}

inline ICommand::CommandType ICommand::getCommandType() const {
	return commandType;
}

void ICommand::setProcess(Process* p)
{
	this->process = p; // Set the process for this command
	if (this->process != nullptr) {
		this->pid = this->process->getPID(); // Update the pid to match the process
	}
}
