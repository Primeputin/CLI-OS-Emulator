#include "Process.h"

Process::Process(int pid, string name, std::vector<std::shared_ptr<ICommand>> commandList)
{
	this->pid = pid;
	this->name = name;
	this->currentLine = 0;
	this->totalLines = commandList.size();
	this->createdTime = time(nullptr);
}

bool Process::isFinished() const {
	return this->currentLine >= totalLines;
}

int Process::getRemainingLines() const {
	return totalLines - currentLine;
}

Process::ProcessState Process::getProcessState() const {
	return this->processState;
}

void Process::setProcessState(ProcessState newState)
{
	this->processState = newState;
}

void Process::addCommand(std::shared_ptr<ICommand> command) {
	commandList.push_back(command);
}

void Process::executeCurrentCommand() const {
	if (!commandList.empty() && currentLine < commandList.size()) {
		this->commandList[this->currentLine]->execute();
	}
}

void Process::moveToNextLine() {
	if (currentLine < totalLines) {
		currentLine++;
	}
}

void Process::setCPUCoreID(int coreID)
{
	this->cpuCoreID = coreID;
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

time_t Process::getCreatedTime() const {
	return createdTime;
}

int Process::getCPUCoreID()
{
	return cpuCoreID;
}
