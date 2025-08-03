#pragma once
#include "ICommand.h"
#include "Process.h"
#include <iostream>

class WriteCommand : public ICommand
{
private:
	string valueToBeWritten;
	string address;
public:
	WriteCommand(int pid, std::string address, string valueToBeWritten, Process* process);
	void execute() override;
	void logExecute(int cpuCoreID, std::string fileName) override;
};



