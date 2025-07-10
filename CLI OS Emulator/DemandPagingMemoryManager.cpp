#include "DemandPagingMemoryManager.h"
DemandPagingMemoryManager::DemandPagingMemoryManager(uint32_t maxOverallMemory, uint32_t memoryPerFrame, string backingStorePath)
{
    this->maxOverAllMemory = maxOverallMemory;
	this->memoryPerFrame = memoryPerFrame;
	this->maxPhysicalPages = maxOverallMemory / memoryPerFrame; 
    memoryMap.resize(maxPhysicalPages);
    this->backingStorePath = backingStorePath;
    backingStore.open(backingStorePath, std::ios::in | std::ios::out | std::ios::binary);
    if (!backingStore.is_open()) {
        std::cerr << "Failed to open backing store file." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

bool DemandPagingMemoryManager::allocate(shared_ptr<Process> process) {
    std::lock_guard<std::mutex> lock(memoryLock);
    int pid = process->getPID();
    if (pageTables.find(pid) != pageTables.end()) return true; 

    pageTables[pid] = {};
	symbolTables[pid] = {};

    return true;
}

void DemandPagingMemoryManager::deallocate(int pid) {
    std::lock_guard<std::mutex> lock(memoryLock);

    // Remove all pages from memory map
    for (size_t i = 0; i < memoryMap.size(); ++i) {
        if (memoryMap[i] && memoryMap[i]->pid == pid) {
            pageOut(i);
        }
    }
    pageTables.erase(pid);
    symbolTables.erase(pid);
}

void DemandPagingMemoryManager::visualizeMemory(uint64_t currentCycle) {

    std::lock_guard<std::mutex> lock(memoryLock);
    std::cout << "[Memory at Cycle " << currentCycle << "]\n";
    for (size_t i = 0; i < memoryMap.size(); ++i) {
        if (memoryMap[i]) {
            std::cout << "Frame " << i << ": PID " << memoryMap[i]->pid
                << ", VPN " << memoryMap[i]->virtualPageNumber << "\n";
        }
        else {
            std::cout << "Frame " << i << ": Empty\n";
        }
    }
}

shared_ptr<Process> DemandPagingMemoryManager::getOldestPreemptedProcess()
{
    return  nullptr;
}

bool DemandPagingMemoryManager::isProcessAllocated(std::shared_ptr<Process> process) {
    return pageTables.find(process->getPID()) != pageTables.end();
}

void DemandPagingMemoryManager::printMemoryStats()
{
    std::cout << "Pages Paged In: " << numPagedIn << std::endl;
    std::cout << "Pages Paged Out: " << numPagedOut << std::endl;
}

string DemandPagingMemoryManager::accessVariable(int pid, const std::string& varName)
{
    std::lock_guard<std::mutex> lock(memoryLock);
    auto& symTable = symbolTables[pid];
    if (symTable.find(varName) == symTable.end()) return "UNDEFINED";

    int virtualPage = symTable[varName];

    if (pageTables[pid].find(virtualPage) == pageTables[pid].end()) {
        pageIn(pid, virtualPage);
    }

    updateLRU(pid, virtualPage);
    return "ValueOf(" + varName + ")";
}

void DemandPagingMemoryManager::pageIn(int pid, int virtualPage)
{
    std::lock_guard<std::mutex> lock(memoryLock);

    int physicalPage = findFreeOrLRUPage();

    // Seek backing store
	// 65536 is temporary number of virtual pages per process, adjust as needed
    backingStore.seekg((pid * 65536 + virtualPage) * this->memoryPerFrame); // example offset formula
    char buffer[64];
    backingStore.read(buffer, sizeof(buffer));

    // Load into memoryMap
    memoryMap[physicalPage] = Page{ pid, virtualPage };

    // Update page table
    pageTables[pid][virtualPage] = physicalPage;

    // Update LRU
    updateLRU(pid, virtualPage);
    ++numPagedIn;
}

void DemandPagingMemoryManager::pageOut(int physicalPageIndex)
{
    auto page = memoryMap[physicalPageIndex];
    if (!page.has_value()) return;

    int pid = page->pid;
    int virtualPage = page->virtualPageNumber;

    // Simulate writing to disk
    // 65536 is temporary number of virtual pages per process, adjust as needed
    backingStore.seekp((pid * 65536 + virtualPage) * this->memoryPerFrame);
    char buffer[64] = { 0 }; // Your simulated page contents
    backingStore.write(buffer, sizeof(buffer));

    // Remove from page table
    pageTables[pid].erase(virtualPage);
    memoryMap[physicalPageIndex].reset();

    ++numPagedOut;
}

int DemandPagingMemoryManager::findFreeOrLRUPage()
{
    for (size_t i = 0; i < memoryMap.size(); ++i) {
        if (!memoryMap[i].has_value()) return static_cast<int>(i);
    }

    // Evict LRU
    auto [pid, vpn] = lruQueue.front();
    lruQueue.pop_front();

    int physicalPage = pageTables[pid][vpn];
    pageOut(physicalPage);
    return physicalPage;
}

void DemandPagingMemoryManager::updateLRU(int pid, int virtualPage)
{
    lruQueue.remove({ pid, virtualPage });
    lruQueue.push_back({ pid, virtualPage });
}
