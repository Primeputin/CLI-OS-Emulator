#include "PrintCommand.h"
#include <iostream>

PrintCommand::PrintCommand(int pid, string toPrint)
	: ICommand(pid, ICommand::PRINT), toPrint(toPrint)
{
}

void PrintCommand::execute()
{

	cout << "PID" << this->pid << ": " << this->toPrint << endl;
}