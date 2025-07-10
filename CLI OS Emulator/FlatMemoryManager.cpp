#include "FlatMemoryManager.h"
#include <fstream>
#include <ctime>
#include <iomanip>
#include <filesystem>
namespace fs = std::filesystem;


FlatMemoryManager::FlatMemoryManager(uint32_t maxOverallMemory) : MemoryManager()
{

    this->maxOverAllMemory = maxOverallMemory;

    memoryPartitions.push_back({ 0, maxOverAllMemory , true, nullptr });
}

bool FlatMemoryManager::allocate(shared_ptr<Process> process)
{

    uint32_t processSize = process->getMemorySize();

    for (auto& partition : memoryPartitions) {
        if (partition.isAllocatable && (partition.end - partition.start + 1) >= processSize) {
            uint32_t allocatedEnd = partition.start + processSize - 1;

            partition.isAllocatable = false;
            partition.process = process;

            uint32_t originalEnd = partition.end;
            partition.end = allocatedEnd;

            uint32_t remainingStart = allocatedEnd + 1;
            if (remainingStart <= originalEnd) {
                memoryPartitions.push_back({ remainingStart, originalEnd, true, nullptr });
            }

            numPagedIn++;
            return true;
        }
    }
    return false;
}

void FlatMemoryManager::deallocate(int pid)
{
    for (auto& partition : memoryPartitions) {
        if (partition.process && partition.process->getPID() == pid) {

            partition.isAllocatable = true;
            partition.process = nullptr;

            sort(memoryPartitions.begin(), memoryPartitions.end(),
                [](const MemoryPartition& a, const MemoryPartition& b) {
                    return a.start < b.start;
                });

            mergeFreeMemoryPartitions();
            numPagedOut++;
            return;
        }
    }
    cout << "Process with PID " << pid << " not found in memory.\n";
}

void FlatMemoryManager::visualizeMemory(uint64_t currentCycle)
{
    fs::create_directory("memory_logs");      
    std::ostringstream filename;
    filename << "memory_logs/memory_stamp_" << currentCycle << ".txt";

    std::ofstream out(filename.str());
    if (!out.is_open()) return;

    // Timestamp
    time_t now = time(nullptr);
    tm local_time = {};
    localtime_s(&local_time, &now);
    out << "Timestamp: (" << std::put_time(&local_time, "%m/%d/%Y %I:%M:%S%p") << ")\n";

    // Count processes in memory
    int activeProcessCount = 0;
    uint32_t externalFragmentation = 0;
    for (const auto& partition : memoryPartitions) {
        if (!partition.isAllocatable) activeProcessCount++;
        else externalFragmentation += (partition.end - partition.start);
    }

    out << "Number of processes in memory: " << activeProcessCount << "\n";
    out << "Total external fragmentation in KB: " << externalFragmentation << "\n\n";

    out << "----end---- = " << maxOverAllMemory << "\n\n";

    // Print memory partitions from top to bottom
    std::vector<MemoryPartition> sortedPartitions = memoryPartitions;
    std::sort(sortedPartitions.begin(), sortedPartitions.end(),
        [](const MemoryPartition& a, const MemoryPartition& b) {
            return a.end > b.end;
        });

    for (const auto& partition : sortedPartitions) {
        if (!partition.isAllocatable && partition.process) {
            out << partition.end << "\n";
            out << partition.process->getName() << "\n";
            out << partition.start << "\n\n";
        }
    }

    out << "----start---- = 0\n";
    out.close();
}

void FlatMemoryManager::printMemoryStats()
{
    
}

shared_ptr<Process> FlatMemoryManager::getOldestPreemptedProcess()
{
    return nullptr;
}

void FlatMemoryManager::mergeFreeMemoryPartitions()
{
    for (size_t i = 0; i < memoryPartitions.size() - 1; ++i) {
        auto& current = memoryPartitions[i];
        auto& next = memoryPartitions[i + 1];

        if (current.isAllocatable && next.isAllocatable && (current.end + 1 == next.start)) {
            current.end = next.end;
            memoryPartitions.erase(memoryPartitions.begin() + i + 1);
            --i;
        }
    }
}

bool FlatMemoryManager::isProcessAllocated(shared_ptr<Process> process)
{
    for (const auto& partition : memoryPartitions) {
        if (partition.process == process) {
            return true;
        }
    }
    return false;
}
