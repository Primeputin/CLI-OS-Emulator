#include "CPUCoreWorker.h"
#include <thread>
#include <iostream>

CPUCoreWorker::CPUCoreWorker(int coreID, uint64_t quantumCycles, uint64_t batchProcessFreq, uint64_t minIns, uint64_t maxIns, uint64_t delayPerExecution, shared_ptr<binary_semaphore> startSem, shared_ptr<binary_semaphore> endSem)
	: coreID(coreID), quantumCycles(quantumCycles), batchProcessFreq(batchProcessFreq),
	minIns(minIns), maxIns(maxIns), delayPerExecution(delayPerExecution), startSem(startSem), endSem(endSem), running(false)
{

	thread(&CPUCoreWorker::run, this).detach(); // thread for running the process

}

void CPUCoreWorker::doProcess(std::shared_ptr<class Process> process)
{
	lock_guard<mutex> lock(mtx);
	if (process == nullptr) {
		return; // Handle null process case
	}
	this->currentProcess = process;
	this->running.store(true);

	process->setCPUCoreID(this->coreID); // Set the CPU core ID for the process
	process->setProcessState(Process::ProcessState::RUNNING);
}

void CPUCoreWorker::run()
{
	while (true)
	{
		startSem->acquire(); // Wait for the signal to start running
		currentCycle++; // Increment the cycle count for this core
		if (currentCycle.load() >= delayPerExecution + 1)
		{
			if (running.load())
			{
				this->currentProcess->executeCurrentCommand(); // Execute the current command of the process
				this->currentProcess->moveToNextLine(); // Move to the next instruction line
				if (this->currentProcess->isFinished())
				{
					this->currentProcess->setProcessState(Process::FINISHED); // Set the process state to finished
					stop();
				}
			}
			currentCycle = 0;
		}
		endSem->release(); // Signal that the core has finished processing
	}
}

void CPUCoreWorker::stop()
{
	lock_guard<mutex> lock(mtx);
	this->running.store(false);
}

int CPUCoreWorker::getCoreID() const
{
	return this->coreID;
}

bool CPUCoreWorker::isRunning() const
{
	return this->running.load();
}
