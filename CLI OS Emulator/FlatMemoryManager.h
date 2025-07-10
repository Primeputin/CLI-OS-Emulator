#pragma once
#include "MemoryManager.h"
#include <iostream>
#include <iomanip>
#include <memory>
#include <string>
#include <sstream> 
#include <vector>
#include <algorithm>

struct MemoryPartition {
	uint32_t start;
	uint32_t end;
	bool isAllocatable;
	std::shared_ptr<Process> process;
};

class FlatMemoryManager : public MemoryManager
{
	public:
		FlatMemoryManager(uint32_t maxOverallMemory);
		bool allocate(shared_ptr<Process> process) override;
		void deallocate(int pid) override;
		void visualizeMemory(uint64_t currentCycle) override;
		shared_ptr<Process> getOldestPreemptedProcess();
		void mergeFreeMemoryPartitions();
		bool isProcessAllocated(std::shared_ptr<Process> process) override;
		void printMemoryStats();

	private:
		vector<MemoryPartition> memoryPartitions;
		uint32_t maxOverAllMemory;
		uint32_t numPagedIn = 0;
		uint32_t numPagedOut = 0;
};

