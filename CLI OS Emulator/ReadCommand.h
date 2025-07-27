#pragma once

#include "ICommand.h"
#include "Process.h"
#include <iostream>

class ReadCommand : public ICommand
{

private:
	std::string varName;
	std::string address;
public:
	ReadCommand(int pid, const string varName, string address, Process* process);

	void execute() override;

	void logExecute(int cpuCoreID, std::string fileName) override;

};