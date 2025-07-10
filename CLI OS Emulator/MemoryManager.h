#pragma once

#include <memory>
#include "Process.h"

class MemoryManager
{
    public:
        MemoryManager();
        virtual bool allocate(std::shared_ptr<Process> process) = 0;
        virtual void deallocate(int pid) = 0;
        virtual void visualizeMemory(uint64_t currentCycle) = 0;
        virtual void printMemoryStats() = 0;
        virtual bool isProcessAllocated(std::shared_ptr<Process> process) = 0;
        virtual ~MemoryManager() = default;
};

