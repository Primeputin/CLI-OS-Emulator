#pragma once
#include <memory>
#include <atomic>
#include <mutex>
#include "Process.h"

using namespace std;
class CPUCoreWorker
{
	public:
		CPUCoreWorker(int coreID, int quantumCycles, int batchProcessFreq, int minIns, int maxIns, int delayPerExecution);

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

		shared_ptr<class Process> currentProcess; // process being processed
		bool running = false;
		mutex mtx;


};

