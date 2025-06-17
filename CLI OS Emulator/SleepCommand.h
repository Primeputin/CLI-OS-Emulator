#pragma once

#include "ICommand.h"
#include "Process.h"

class SleepCommand : public ICommand
{
    public:
        SleepCommand(int pid, uint8_t ticks, Process* process);

        void execute() override;

        void logExecute(int cpuCoreID, std::string fileName) override;

        void setSleepTicks(uint8_t ticks);
        bool isSleeping() const;
        void decrementSleepTick();

    private:
        uint8_t ticks;
};

