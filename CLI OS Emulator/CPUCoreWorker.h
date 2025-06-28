#pragma once
#include <memory>
#include <atomic>
#include <mutex>
#include <semaphore>
#include "constants.h"
#include "Process.h"

using namespace std;
class CPUCoreWorker
{
	public:
		CPUCoreWorker(int coreID, uint64_t quantumCycles, uint64_t batchProcessFreq, uint64_t minIns, uint64_t maxIns, uint64_t delayPerExecution, shared_ptr<binary_semaphore> startSem, shared_ptr<binary_semaphore> endSem);

		void doProcess(shared_ptr<class Process> process);
		void run();
		void stop();

		int getCoreID() const;
		bool isRunning() const;
		void stopCoreThread();
		shared_ptr<class Process> getCurrentProcess() const;
		bool shouldInterrupt() const;
		void setProcessBackToReadyState();

	private:
		int coreID;
		uint64_t quantumCycles;
		uint64_t batchProcessFreq;
		uint64_t minIns;
		uint64_t maxIns;
		uint64_t delayPerExecution;
		atomic<uint64_t> currentCycle;
		atomic<uint64_t> currentQuantumCycles;
		shared_ptr<class Process> currentProcess; // process being processed
		atomic<bool> running = false; // boolean for saying it's idle or not
		mutable mutex mtx;
		shared_ptr<binary_semaphore> startSem;
		shared_ptr<binary_semaphore> endSem;
		thread coreThread;
		atomic<bool> coreThreadRunning = true; // boolean for actually shutting or running the cpu


};

