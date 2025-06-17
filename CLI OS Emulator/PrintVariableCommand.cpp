#include "PrintVariableCommand.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include "Process.h"

PrintVariableCommand::PrintVariableCommand(int pid, string varName, Process* process)
	: ICommand(pid, ICommand::PRINT), varName(varName)
{
	this->process = process;
}

void PrintVariableCommand::execute()
{
	uint16_t value;
	process->getVariableValue(this->varName, value);
	cout << value << " from: " << this->varName << endl;
}

void PrintVariableCommand::logExecute(int cpuCoreID, string fileName)
{
	uint16_t value;
	process->getVariableValue(this->varName, value);
	ofstream outFile("output/" + fileName + ".txt", ios::app); // Create and open a file for writing

	if (outFile.is_open()) {
		time_t now = time(0);
		tm localTime;
		localtime_s(&localTime, &now);
		outFile << "(" << put_time(&localTime, "%m/%d/%Y %I:%M:%S%p") << ")"
			<< setw(6) << "Core:" << setw(2) << cpuCoreID << "  " << value << " from: " << this->varName << endl;

		outFile.close();
	}
	else {
		cout << "Unable to open file for writing." << endl;
	}
}