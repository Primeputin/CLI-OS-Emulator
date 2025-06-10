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
		CPUCoreWorker(int coreID, int quantumCycles, int batchProcessFreq, int minIns, int maxIns, int delayPerExecution, shared_ptr<binary_semaphore> startSem, shared_ptr<binary_semaphore> endSem);

		void doProcess(shared_ptr<class Process> process);
		void run();
		void stop();

		int getCoreID() const;
		bool isRunning() const;

	private:
		int coreID;
		int quantumCycles;
		int batchProcessFreq;
		int minIns;
		int maxIns;
		int delayPerExecution;
		atomic<uint64_t> currentCycle;

		shared_ptr<class Process> currentProcess; // process being processed
		// bool running = false;
		atomic<bool> running = false;
		mutable mutex mtx;
		shared_ptr<binary_semaphore> startSem;
		shared_ptr<binary_semaphore> endSem;


};

