#pragma once
#include "ICommand.h"
#include "Process.h"
#include <iostream>

class WriteCommand : public ICommand
{
private:
	uint16_t value;
	std::string address;
public:
	WriteCommand(int pid, std::string address, uint16_t value, Process* process);
	void execute() override;
	void logExecute(int cpuCoreID, std::string fileName) override;
};



