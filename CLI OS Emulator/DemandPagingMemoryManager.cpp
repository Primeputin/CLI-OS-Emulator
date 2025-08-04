#include "DemandPagingMemoryManager.h"
DemandPagingMemoryManager::DemandPagingMemoryManager(uint32_t maxOverallMemory, uint32_t memoryPerFrame, string backingStorePath)
{
    this->maxOverAllMemory = maxOverallMemory;
	this->memoryPerFrame = memoryPerFrame;
	this->maxPhysicalPages = maxOverallMemory / memoryPerFrame; 
	this->backingStorePath = backingStorePath;
    memoryMap.resize(maxPhysicalPages);
    for (uint32_t i = 0; i < maxPhysicalPages; i++) {
        memoryMap[i] = Frame{-1, vector<uint8_t>(this->memoryPerFrame, 0)};
    }
    std::filesystem::remove(backingStorePath);
    this->backingStorePath = backingStorePath;
}

bool DemandPagingMemoryManager::allocate(shared_ptr<Process> process)
{
    
    std::lock_guard<std::mutex> lock(memoryLock);
    
    for (uint32_t i = 0; i < process->getNPages(); i++)
    {
		/*if (i >= maxPhysicalPages) {
			throw std::runtime_error("Not enough physical pages available for the process");
		}*/
		pageTables[process->getPID()][i] = PageTableEntry{ -1, false }; // Initialize the page table entry for this process
	    backStoreFrame(process->getPID(), i, Frame(-1, vector<uint8_t>(this->memoryPerFrame, 0))); // Initialize the backing store for this process
    }

    return true;
}

void DemandPagingMemoryManager::deallocate(int pid) 
{
    std::lock_guard<std::mutex> lock(memoryLock);
    for (uint32_t i = 0; i < maxPhysicalPages; i++) 
    {
        if (memoryMap[i].pid == pid)
        {
			memoryMap[i].pid = -1; // Mark as unoccupied
			memoryMap[i].values.assign(memoryPerFrame, 0); // Reset values
			pageTables.erase(pid); // Remove the page table entry for this process
			symbolTables.erase(pid); // Remove the symbol table entry for this process
			
        }
    }
    lruQueue.remove_if([pid](const std::pair<int, int16_t>& entry) {
        return entry.first == pid; // Remove all entries for this process
        });

}

void DemandPagingMemoryManager::visualizeMemory(uint64_t currentCycle) {

    std::lock_guard<std::mutex> lock(memoryLock);
    
}

bool DemandPagingMemoryManager::isProcessAllocated(std::shared_ptr<Process> process) // placed in the back store basically
{
    std::lock_guard<std::mutex> lock(memoryLock);
	if (pageTables.find(process->getPID()) != pageTables.end() && !pageTables[process->getPID()].empty())
	{
		return true; 
	}
    return false;
}

void DemandPagingMemoryManager::printMemoryStats(uint64_t activeTicks, uint64_t idleTicks)
{
    cout << "-------------------------------------------------" << endl;
    cout << "| VMSTAT                                        |" << endl;
    cout << "-------------------------------------------------" << endl;
    cout << setw(9) << maxOverAllMemory << "  Total Memory" << endl;

    uint32_t usedFrames = 0;
    for (int i = 0; i < memoryMap.size(); i++)
    {
        if (memoryMap[i].pid != -1) {
            usedFrames++;
        }
    }
    cout << setw(9) << usedFrames * memoryPerFrame << "  Used Memory" << endl;
    cout << setw(9) << (this->maxPhysicalPages - usedFrames) * memoryPerFrame << "  Free Memory" << endl;
	cout << "-------------------------------------------------" << endl;
    cout << setw(9) << idleTicks << "  Idle CPU ticks" << endl;
    cout << setw(9) << activeTicks << "  Active CPU ticks" << endl;
    cout << setw(9) << idleTicks + activeTicks << "  Total CPU ticks" << endl;
    cout << "Pages Paged In: " << numPagedIn << endl;
    cout << "Pages Paged Out: " << numPagedOut << endl;
    cout << "<<------------------------------------------------->>" << endl;
}

// not being used rn
// this also includes memory used by processes not running currently
uint32_t DemandPagingMemoryManager::memoryUsage()
{
    std::lock_guard<std::mutex> lock(memoryLock);

    uint32_t usedMemory = 0;
	for (const auto& pid : pageTables) {
		for (const auto& entry : pid.second) {
			if (entry.second.valid) {
				usedMemory += memoryPerFrame; // Each valid page contributes memoryPerFrame bytes
			}
		}
	}

    return usedMemory;
}

uint32_t DemandPagingMemoryManager::memoryUsage(int pid)
{
	std::lock_guard<std::mutex> lock(memoryLock);
	if (pageTables.find(pid) == pageTables.end()) {
		return 0; // Process not found
	}
	uint32_t usedMemory = 0;
    for (const auto& entry : pageTables[pid]) {
		if (entry.second.valid) {
			usedMemory += memoryPerFrame; // Each valid page contributes memoryPerFrame bytes
		}
    }
    return usedMemory;
}

uint32_t DemandPagingMemoryManager::memoryUsagePercentage(uint32_t memoryUsage)
{
    return memoryUsage * 100.0 / this->maxOverAllMemory;
}

void DemandPagingMemoryManager::loadAddress(int pid, uint32_t memorySize, uint16_t address)
{
    if (address >= memorySize || address < 0)
    {
        throw std::runtime_error("Invalid address");
    }

    if (pageTables.find(pid) == pageTables.end() || pageTables[pid].empty())
    {
        throw std::runtime_error("Process not allocated or no pages found");
    }
    int16_t virtualPage = address / memoryPerFrame;
    if (!pageTables[pid][virtualPage].valid)
    {
        pageIn(pid, virtualPage);
    }
    updateLRU(pid, virtualPage);
}

uint16_t DemandPagingMemoryManager::getValueFromAddressToVariable(int pid, uint32_t memorySize, string varName)
{
	if (symbolTables.find(pid) == symbolTables.end() || symbolTables[pid].empty())
	{
		throw std::runtime_error("No variables found for this process");
	}
	getValueFromAddress(pid, memorySize, symbolTables[pid][varName].virtualAddress);
}

uint16_t DemandPagingMemoryManager::getValueFromAddress(int pid, uint32_t memorySize, uint16_t address)
{
	std::lock_guard<std::mutex> lock(memoryLock);
       
    try {
        
		loadAddress(pid, memorySize, address);
        uint16_t nextAddress = address + 1;
		loadAddress(pid, memorySize, nextAddress);

        auto offset = address % memoryPerFrame;
		auto offsetOfNextAddress = nextAddress % memoryPerFrame;

        auto physicalPage = pageTables[pid][address / memoryPerFrame].physicalPage;
		auto physicalPageOfNextAddress = pageTables[pid][nextAddress / memoryPerFrame].physicalPage;
        uint8_t firstByte = memoryMap[physicalPage].values[offset];
        uint8_t secondByte = memoryMap[physicalPageOfNextAddress].values[offsetOfNextAddress];
        
		uint16_t value = into2Bytes(firstByte, secondByte);

        return value;
    }
    catch (const std::runtime_error& e){

        return 0;
    }

    return 0;

}

void DemandPagingMemoryManager::writeValueToVariable(int pid, uint32_t memorySize, uint16_t value, string varName)
{
    if (symbolTables.find(pid) == symbolTables.end() || symbolTables[pid].empty())
    {
        throw std::runtime_error("No variables found for this process");
    }
    writeValueToMemory(pid, memorySize, value, symbolTables[pid][varName].virtualAddress);
}

void DemandPagingMemoryManager::writeValueToMemory(int pid, uint32_t memorySize, uint16_t value, uint16_t address)
{
	std::lock_guard<std::mutex> lock(memoryLock);

    try {
		loadAddress(pid, memorySize, address);

        uint16_t nextAddress = address + 1;
        uint8_t firstByte = value & 0xFF;
        uint8_t secondByte = (value >> 8) & 0xFF;

		loadAddress(pid, memorySize, nextAddress);

        auto offset = address % memoryPerFrame;
        auto offsetOfNextAddress = nextAddress % memoryPerFrame;

        auto physicalPage = pageTables[pid][address / memoryPerFrame].physicalPage;
        auto physicalPageOfNextAddress = pageTables[pid][nextAddress / memoryPerFrame].physicalPage;

        memoryMap[physicalPage].values[offset] = firstByte;
        memoryMap[physicalPageOfNextAddress].values[offsetOfNextAddress] = secondByte;
	}
    catch (const std::out_of_range& e) {
        throw std::runtime_error("Memory access out of range");
    }
}


void DemandPagingMemoryManager::loadVariable(int pid, int maxNumOfVars, const std::string& varName, uint16_t value)
{
	std::lock_guard<std::mutex> lock(memoryLock);
    
    if (symbolTables[pid].size() >= maxNumOfVars)
    {
		return; // Limit to 32 variables per process
    }

    int16_t pageToBePlaced = -1;
    if (symbolTables[pid].find(varName) != symbolTables[pid].end()) // if the variable already exists
    {
		pageToBePlaced = symbolTables[pid][varName].virtualPage;
    }
    else
    {
	    pageToBePlaced = symbolTables[pid].size() * 2 / memoryPerFrame; // each uint16_t variable takes two bytes
	    symbolTables[pid][varName] = SymbolReference{ pageToBePlaced, static_cast<uint16_t>(symbolTables[pid].size() * 2) };
    }

	if (!pageTables[pid][pageToBePlaced].valid) 
    {
		pageIn(pid, pageToBePlaced);
	}

	updateLRU(pid, pageToBePlaced);
	auto offset = symbolTables[pid][varName].virtualAddress % memoryPerFrame; 
	uint8_t firstByte = value & 0xFF;
	uint8_t secondByte = (value >> 8) & 0xFF;
    auto physicalPage = pageTables[pid][pageToBePlaced].physicalPage;
	memoryMap[physicalPage].values[offset] = firstByte;
	memoryMap[physicalPage].values[offset + 1] = secondByte;
}

uint16_t DemandPagingMemoryManager::accessVariable(int pid, const std::string& varName)
{
    std::lock_guard<std::mutex> lock(memoryLock);
    int16_t virtualPage = -1;
    if (!pageTables[pid].empty() && symbolTables[pid].find(varName) != symbolTables[pid].end()) 
    {
		virtualPage = symbolTables[pid][varName].virtualPage;

        if (!pageTables[pid][virtualPage].valid)
        {
            pageIn(pid, virtualPage);
        }
        updateLRU(pid, virtualPage);
        auto offset = symbolTables[pid][varName].virtualAddress % memoryPerFrame;
        auto physicalPage = pageTables[pid][virtualPage].physicalPage;
        uint8_t firstByte = memoryMap[physicalPage].values[offset];
        uint8_t secondByte = memoryMap[physicalPage].values[offset + 1];
        return into2Bytes(firstByte, secondByte);	
        
    }
    return 0;
    
}

void DemandPagingMemoryManager::backStoreFrame(int pid, int virtualPage, const Frame& frame) {
    
    std::ofstream out(backingStorePath, std::ios::app); // append mode
    if (!out) throw std::runtime_error("Failed to open file for writing");
    if (frame.values.size() % 2 != 0) {
        throw std::runtime_error("Frame size must be even to pack into uint16_t");
    }

    out << "# PID: " << pid << " PAGE: " << virtualPage << "\n";
    
    for (uint32_t i = 0; i < frame.values.size(); ++i) {
        out << static_cast<int16_t>(frame.values[i]);
        if (i != frame.values.size() - 1) out << "\n";
    }
    
    out << "\n";
    out.close();


}

DemandPagingMemoryManager::Frame DemandPagingMemoryManager::loadFrameFromBackingStore(int pid, int virtualPage) 
{
    std::ifstream in(backingStorePath);
    if (!in) throw std::runtime_error("Failed to open backing store file");

    std::ostringstream newFileContent;
    std::string line;

    Frame result;
    bool found = false;
    bool inTargetBlock = false;
	
    while (std::getline(in, line)) {
        if (line.starts_with("# PID:")) {
            std::istringstream header(line);
            std::string tmp;
            int filePid, filePage;
            header >> tmp >> tmp >> filePid >> tmp >> filePage;

            if (filePid == pid && filePage == virtualPage) {
                found = true;
                inTargetBlock = true;
				result.pid = pid;
                continue; // skip this header (don’t write it back)
            }
            else {
                inTargetBlock = false;
                newFileContent << line << "\n"; // keep unrelated header
            }
        }
        else if (inTargetBlock && result.values.size() < memoryPerFrame)
        {
            int value = std::stoi(line);
            result.values.push_back(value);
        }
        else {
            newFileContent << line << "\n";
        }
    }

    in.close();

    if (!found)
    {
        throw std::runtime_error("Frame not found in backing store");
    }


    // Rewrite the file without the loaded frame
    std::ofstream out(backingStorePath, std::ios::trunc);
    out << newFileContent.str();
    out.close();
	
    return result;
   
}

uint16_t DemandPagingMemoryManager::into2Bytes(uint8_t first, uint8_t second)
{
    uint16_t combined = (uint16_t)first | ((uint16_t)second << 8);
    return combined;
}

void DemandPagingMemoryManager::pageIn(int pid, int16_t virtualPage)
{
    int physicalPage = findFreeOrLRUPage(virtualPage);
    memoryMap[physicalPage] = loadFrameFromBackingStore(pid, virtualPage);
    pageTables[pid][virtualPage].physicalPage = physicalPage;
	pageTables[pid][virtualPage].valid = true;
    numPagedIn++;
}

void DemandPagingMemoryManager::pageOut(int16_t physicalPageIndex, int16_t virtualPage)
{
    Frame frame = memoryMap[physicalPageIndex];
    if (frame.pid < 0) return;

    auto pid = frame.pid;
	
	
	backStoreFrame(pid, virtualPage, frame);

    // Remove from page table
	pageTables[pid][virtualPage].valid = false; // Mark as invalid
    memoryMap[physicalPageIndex].pid = -1;

    ++numPagedOut;
}

uint32_t DemandPagingMemoryManager::findFreeOrLRUPage(int16_t virtualPage)
{
    for (uint32_t i = 0; i < memoryMap.size(); i++) {
        if (memoryMap[i].pid < 0) {
            return i; 
        }
    }

    std::pair<int, int16_t> victim = lruQueue.front();
    lruQueue.pop_front();

    int evictedPid = victim.first;
    int evictedVpn = victim.second;

    PageTableEntry pageTableEntry = pageTables[evictedPid][evictedVpn];

	auto physicalPage = pageTableEntry.physicalPage;

    pageOut(physicalPage, virtualPage);
	pageTables[evictedPid][evictedVpn].physicalPage = -1; // Mark as invalid
	pageTables[evictedPid][evictedVpn].valid = false; // Mark as invalid
    return physicalPage;
}

void DemandPagingMemoryManager::updateLRU(int pid, int virtualPage)
{
    lruQueue.remove({ pid, virtualPage });
    lruQueue.push_back({ pid, virtualPage });
}
