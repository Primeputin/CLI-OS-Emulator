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
#include <filesystem>

class DemandPagingMemoryManager : public MemoryManager
{
	struct Frame {
		int pid = -1; // Process ID that owns this frame
		vector<uint8_t> values; // supposed to be 8 bytes but just made it int16_t to mark -1 as not occupied
	};

	struct PageTableEntry {
		int16_t physicalPage = -1;
		bool valid = false;
	};

	struct SymbolReference {
		int16_t virtualPage;
		uint16_t virtualAddress; 
	};

	public:
		DemandPagingMemoryManager(uint32_t maxOverallMemory, uint32_t memoryPerFrame, string backingStorePath);
		bool allocate(std::shared_ptr<Process> process) override;
		void deallocate(int pid) override;
		void visualizeMemory(uint64_t currentCycle) override;
		bool isProcessAllocated(std::shared_ptr<Process> process) override;
		void printMemoryStats(uint64_t activeTicks , uint64_t idleTicks);
		uint32_t memoryUsage();
		uint32_t memoryUsage(int pid);
		uint32_t memoryUsagePercentage(uint32_t memoryUsage);

		void loadAddress(int pid, uint32_t memorySize, uint16_t address);
		uint16_t getValueFromAddressToVariable(int pid, uint32_t memorySize, string varName);
		uint16_t getValueFromAddress(int pid, uint32_t memorySize, uint16_t address);
		void writeValueToVariable(int pid, uint32_t memorySize, uint16_t value, string varName);
		void writeValueToMemory(int pid, uint32_t memorySize, uint16_t value, uint16_t address);

		void loadVariable(int pid, int maxNumOfVars, const std::string& varName, uint16_t value);
		uint16_t accessVariable(int pid, const std::string& varName);
		void backStoreFrame(int pid, int virtualPage, const Frame& frame);
		Frame loadFrameFromBackingStore(int pid, int virtualPage);
	private:
		vector<Frame> memoryMap;
		uint32_t maxOverAllMemory;
		uint32_t memoryPerFrame;
		uint32_t maxPhysicalPages;
		uint32_t numPagedIn = 0;
		uint32_t numPagedOut = 0;

		unordered_map<int, unordered_map<int16_t, PageTableEntry>> pageTables; // pid -> (virtual page -> physical page and valid bit)
		unordered_map<int, unordered_map<string, SymbolReference>> symbolTables; // pid -> varName -> virtualPage and the virtualAddress
		std::list<pair<int, int16_t>> lruQueue; // (pid, virtualPage)
		std::string backingStorePath;

		std::mutex memoryLock;

		uint16_t into2Bytes(uint8_t first, uint8_t second); // little endian
		void pageIn(int pid, int16_t virtualPage);
		void pageOut(int16_t physicalPageIndex, int16_t virtualPage);
		uint32_t findFreeOrLRUPage(int16_t virtualPage);
		void updateLRU(int pid, int virtualPage);

};

