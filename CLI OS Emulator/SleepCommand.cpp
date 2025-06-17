#include "SleepCommand.h"
#include <iostream>
SleepCommand::SleepCommand(int pid, uint8_t ticks, Process* process)
	: ICommand(pid, CommandType::SLEEP), ticks(ticks) {
	setProcess(process); // Set the process for this command
}

void SleepCommand::execute()
{
	if (process != nullptr) {
	    setSleepTicks(ticks);
		this->process->setProcessState(Process::WAITING); // Yield CPU
	}
}

void SleepCommand::logExecute(int cpuCoreID, std::string fileName)
{
	execute();
}

void SleepCommand::setSleepTicks(uint8_t ticks)
{
	this->ticks = ticks;
}

bool SleepCommand::isSleeping() const
{
	return this->ticks > 0;
}

void SleepCommand::decrementSleepTick()
{
	if (this->ticks > 0)
		--this->ticks;
}
