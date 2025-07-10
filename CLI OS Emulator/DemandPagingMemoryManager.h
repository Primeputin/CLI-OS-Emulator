#pragma once
#include "MemoryManager.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <memory>
#include <string>
#include <sstream> 
#include <vector>
#include <unordered_map>
#include <list>
#include <mutex>
#include <optional>

class DemandPagingMemoryManager : public MemoryManager
{
	public:
		DemandPagingMemoryManager(uint32_t maxOverallMemory, uint32_t memoryPerFrame, string backingStorePath);
		bool allocate(std::shared_ptr<Process> process) override;
		void deallocate(int pid) override;
		void visualizeMemory(uint64_t currentCycle) override;
		bool isProcessAllocated(std::shared_ptr<Process> process) override;
		std::shared_ptr<Process> getOldestPreemptedProcess(); // Optional
		void printMemoryStats();

		// Access or modify a variable (will trigger page fault if needed)
		string accessVariable(int pid, const std::string& varName);

	private:
		struct Page {
			int pid;
			int virtualPageNumber;
		};
		std::vector<std::optional<Page>> memoryMap; // Physical memory map, indexed by physical page index
		std::unordered_map<int, std::unordered_map<int, int>> pageTables; // pid -> (virtual page -> physical page)
		std::unordered_map<int, std::unordered_map<std::string, int>> symbolTables; // pid -> varName -> virtualPage
		uint32_t maxOverAllMemory;
		uint32_t memoryPerFrame;
		uint32_t maxPhysicalPages;
		uint32_t numPagedIn = 0;
		uint32_t numPagedOut = 0;

		std::list<std::pair<int, int>> lruQueue; // (pid, virtualPage)
		std::string backingStorePath;
		std::fstream backingStore;

		std::mutex memoryLock;

		void pageIn(int pid, int virtualPage);
		void pageOut(int physicalPageIndex);
		int findFreeOrLRUPage();
		void updateLRU(int pid, int virtualPage);

};

