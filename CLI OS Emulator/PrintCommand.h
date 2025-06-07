#pragma once
#include <string>
#include "ICommand.h"

using namespace std;

class PrintCommand : public ICommand
{
	public:
		PrintCommand(int pid, string toPrint);
		void execute() override;
		void logExecute(int cpuCoreID, string fileName) const override;
	private:
		string toPrint;
};

