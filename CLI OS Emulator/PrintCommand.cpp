#include "PrintCommand.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>

PrintCommand::PrintCommand(int pid, string toPrint)
	: ICommand(pid, ICommand::PRINT), toPrint(toPrint)
{
}

void PrintCommand::execute()
{
	cout << this->toPrint << endl;
}

void PrintCommand::logExecute(int cpuCoreID, string fileName) const
{
	ofstream outFile("output/" + fileName + ".txt", ios::app); // Create and open a file for writing

	if (outFile.is_open()) {
		time_t now = time(0);
		tm localTime;
		localtime_s(&localTime, &now);
		outFile << "(" << put_time(&localTime, "%m/%d/%Y %I:%M:%S%p") << ")"
			<< setw(6) << "Core:" << setw(2) << cpuCoreID << "  " << this->toPrint << endl;

		outFile.close();
	}
	else {
		cout << "Unable to open file for writing." << endl;
	}
}