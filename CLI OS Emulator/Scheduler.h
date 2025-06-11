#pragma once

#include <string>
#include <memory>
#include <vector>
#include <queue>
#include <ctime>
#include <thread>
#include <atomic>
#include <mutex>
#include <semaphore>
#include "Console.h"
#include "CPUCoreWorker.h"
#include "Process.h"
#include "constants.h"


using namespace std;

class Scheduler
{
	public:
		enum SchedulingAlgorithm
		{
			FCFS, // First Come First Serve
			RR,   // Round Robin
		};
		Scheduler(SchedulingAlgorithm algorithm, int numberOfCores, uint64_t quantumCycles, uint64_t batchProcessFreq, uint64_t minIns, uint64_t maxIns, uint64_t delaysPerExecution);

		void addProcessToReadyQueue(shared_ptr<class Process> process);
		void assignProcessToCore(int coreID);
		void checkFinishedProcesses();
		void generateProcesses();
		void stopGenerationOfProcesses();
		shared_ptr<Console> generateRandomProcess(string name);
		void run();
		void stop();
		void fcfs();
		void printProcessesStatus(std::ostream& out);
		uint64_t getTotalCycles();
		uint64_t getTotalProcesses();
	private:
		int numberOfCores;
		SchedulingAlgorithm algorithm;
		uint64_t quantumCycles;
		uint64_t batchProcessFreq;
		uint64_t minIns;
		uint64_t maxIns;
		uint64_t delayPerExecution;
		atomic <uint64_t> totalCycles = 0; // Total cycles executed by the scheduler
		atomic <uint64_t> totalProcesses = 0; // Total processes created
		atomic <uint64_t> latestProcessID = 0; // Total processes created
		thread schedulerThread;
		atomic<bool> running = false;
		atomic<bool> generate = false;
		std::mutex queueMutex;       // Protects readyQueue
		std::mutex runningMutex;     // Protects runningProcesses
		std::mutex finishedMutex;    // Protects finishedProcesses
		vector<shared_ptr<binary_semaphore>> startSem; // Semaphores for each core
		vector<shared_ptr<binary_semaphore>> endSem; // Semaphores for each core
		vector<unique_ptr<CPUCoreWorker>> cores;
		queue<shared_ptr<Process>> readyQueue;
		vector<shared_ptr<Process>> runningProcesses;
		vector<shared_ptr<Process>> finishedProcesses;
};

