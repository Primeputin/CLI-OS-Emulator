#include "ICommand.h"

ICommand::ICommand(int pid, CommandType commandType)
{
	this->pid = pid;
	this->commandType = commandType;
}

inline ICommand::CommandType ICommand::getCommandType() const {
	return commandType;
}
