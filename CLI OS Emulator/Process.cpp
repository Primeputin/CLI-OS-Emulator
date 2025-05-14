#include "Process.h"

Process::Process(string name, uint16_t currentLine, uint16_t totalLines)
{
	this->name = name;
	this->currentLine = currentLine;
	this->totalLines = totalLines;
}

string Process::getName() const {
    return name;
}

uint16_t Process::getCurrentLine() const {
    return currentLine;
}

uint16_t Process::getTotalLines() const {
    return totalLines;
}
