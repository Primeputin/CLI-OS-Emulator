#include "DemandPagingMemoryManager.h"
DemandPagingMemoryManager::DemandPagingMemoryManager(uint32_t maxOverallMemory, uint32_t memoryPerFrame, string backingStorePath)
{
    this->maxOverAllMemory = maxOverallMemory;
	this->memoryPerFrame = memoryPerFrame;
	this->maxPhysicalPages = maxOverallMemory / memoryPerFrame; 
	this->backingStorePath = backingStorePath;
    memoryMap.resize(maxPhysicalPages);
    for (uint32_t i = 0; i < maxPhysicalPages; i++) {
        memoryMap[i] = Frame{-1, vector<int16_t>(this->memoryPerFrame, -1)};
    }
    std::filesystem::remove(backingStorePath);
    this->backingStorePath = backingStorePath;
}

bool DemandPagingMemoryManager::allocate(shared_ptr<Process> process)
{
    std::lock_guard<std::mutex> lock(memoryLock);
    
    for (uint32_t i = 0; i < process->getNPages(); i++)
    {
	    backStoreFrame(process->getPID(), i, Frame(-1, vector<int16_t>(this->memoryPerFrame, -1))); // Initialize the backing store for this process
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

}

void DemandPagingMemoryManager::visualizeMemory(uint64_t currentCycle) {

    std::lock_guard<std::mutex> lock(memoryLock);

}

bool DemandPagingMemoryManager::isProcessAllocated(std::shared_ptr<Process> process) // placed in the back store basically
{
    std::lock_guard<std::mutex> lock(memoryLock);
	if (pageTables.find(process->getPID()) != pageTables.end() && !pageTables[process->getPID()].empty())
	{
		return true; // Process has pages allocated
	}
    return false;
}

void DemandPagingMemoryManager::printMemoryStats()
{
    std::cout << "Pages Paged In: " << numPagedIn << std::endl;
    std::cout << "Pages Paged Out: " << numPagedOut << std::endl;
}

void DemandPagingMemoryManager::loadVariable(int pid, const std::string& varName, uint16_t value)
{
	std::lock_guard<std::mutex> lock(memoryLock);

    if (symbolTables[pid].size() >= 32)
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
	auto offset = symbolTables[pid][varName].virtualAddress;
	uint8_t firstByte = value & 0xFF;
	uint8_t secondByte = (value >> 8) & 0xFF;
    auto physicalPage = pageTables[pid][pageToBePlaced].physicalPage;
	memoryMap[physicalPage].values[offset] = firstByte;
	memoryMap[physicalPage].values[offset + 1] = secondByte;
}

int16_t DemandPagingMemoryManager::accessVariable(int pid, const std::string& varName)
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
        auto offset = symbolTables[pid][varName].virtualAddress; 
        auto physicalPage = pageTables[pid][virtualPage].physicalPage;
        uint8_t firstByte = memoryMap[physicalPage].values[offset];
        uint8_t secondByte = memoryMap[physicalPage].values[offset + 1];
        return into2Bytes(firstByte, secondByte);	
        
    }
    return -1;
    
}

void DemandPagingMemoryManager::backStoreFrame(int pid, int virtualPage, const Frame& frame) {
    
    std::ofstream out(backingStorePath, std::ios::app); // append mode
    if (!out) throw std::runtime_error("Failed to open file for writing");

    out << "# PID: " << pid << " PAGE: " << virtualPage << "\n";

    const std::vector<int16_t>& bytes = frame.values;
    if (bytes.size() % 2 != 0) {
        throw std::runtime_error("Frame size must be even to pack into uint16_t");
    }

    uint32_t i = 0;
    while (i < bytes.size())
    {
        // means null or unoccupied
        if (bytes[i] < 0) {
            out << -1;
            if (i + 1 < bytes.size()) out << " ";
            i++;
        }
        else
        {
			uint16_t combined = into2Bytes(static_cast<uint8_t>(bytes[i]), static_cast<uint8_t>(bytes[i + 1]));
            out << combined;
            if (i + 2 < bytes.size()) out << " ";
            i += 2;
        }
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

    while (std::getline(in, line) && result.values.size() < memoryPerFrame) {
        if (line.starts_with("# PID:")) {
            std::istringstream header(line);
            std::string tmp;
            int filePid, filePage;
            header >> tmp >> tmp >> filePid >> tmp >> filePage;

            if (filePid == pid && filePage == virtualPage) {
                found = true;
                inTargetBlock = true;
                continue; // skip this header (don’t write it back)
            }
            else {
                inTargetBlock = false;
                newFileContent << line << "\n"; // keep unrelated header
            }
        }
        else if (inTargetBlock) {
            std::istringstream values(line);
            std::string token;

            while (values >> token) {
                if (token == "-1") {
					result.values.push_back(-1); // means null or unoccupied
                }
                else {
                    // parse as uint16_t then unpack
                    uint16_t packed = static_cast<uint16_t>(std::stoi(token));
                    int16_t low = packed & 0xFF;
                    int16_t high = (packed >> 8) & 0xFF;
                    result.values.push_back(low);
                    result.values.push_back(high);
                }
            }

            inTargetBlock = false;
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
    updateLRU(pid, virtualPage);
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
