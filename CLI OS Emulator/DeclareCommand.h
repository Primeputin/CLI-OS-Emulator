#pragma once

#include <string>
#include "Process.h"
#include "ICommand.h"
using namespace std;

class DeclareCommand: public ICommand
{
	public: 
		DeclareCommand(int pid, string& varName, uint16_t value, Process* process);
		void execute() override;
		void logExecute(int cpuCoreID, std::string fileName) override;

	private:
		string varName;
		uint16_t value;

};

