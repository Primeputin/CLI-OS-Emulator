#pragma once
#include <string>
#include "ICommand.h"

using namespace std;

class PrintVariableCommand : public ICommand
{
	public:
		PrintVariableCommand(int pid, string varName, Process* process);
		void execute() override;
		void logExecute(int cpuCoreID, string fileName) override;
	private:
		string varName;
};

