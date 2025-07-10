#include "CPUCoreWorker.h"
#include <thread>
#include <iostream>

CPUCoreWorker::CPUCoreWorker(int coreID, uint64_t quantumCycles, uint64_t batchProcessFreq, uint64_t minIns, uint64_t maxIns, uint64_t delayPerExecution, shared_ptr<binary_semaphore> startSem, shared_ptr<binary_semaphore> endSem)
	: coreID(coreID), quantumCycles(quantumCycles), batchProcessFreq(batchProcessFreq),
	minIns(minIns), maxIns(maxIns), delayPerExecution(delayPerExecution), startSem(startSem), endSem(endSem), running(false)
{
	this->currentQuantumCycles.store(this->quantumCycles);
	coreThread = thread(&CPUCoreWorker::run, this); // thread for running the process

}

void CPUCoreWorker::doProcess(std::shared_ptr<class Process> process)
{
	lock_guard<mutex> lock(mtx);
	if (process == nullptr) {
		return; // Handle null process case
	}
	this->currentProcess = process;
	this->running.store(true);
	this->currentCycle.store(0); // Reset the current cycle count for the new process
	this->currentQuantumCycles.store(this->quantumCycles); // Reset quantum cycles for the new process

	process->setCPUCoreID(this->coreID); // Set the CPU core ID for the process
	process->setProcessState(Process::ProcessState::RUNNING);
}

void CPUCoreWorker::run()
{
	while (coreThreadRunning.load())
	{
		// startSem->acquire(); // Wait for the signal to start running
		currentCycle++; // Increment the cycle count for this core
		if (currentCycle.load() >= delayPerExecution + 1)
		{
			if (running.load())
			{
				
				this->currentProcess->executeCurrentCommand(); // Execute the current command of the process
				if (!this->currentProcess->isSleeping())
				{
					this->currentProcess->moveToNextLine(); // Move to the next instruction line
					if (this->currentProcess->isFinished())
					{
						this->currentProcess->setProcessState(Process::FINISHED); // Set the process state to finished
						stop();
					}
				}
				currentCycle = 0;
			}
		}
		if (running.load())
		{

			this->currentQuantumCycles--; // Decrement the quantum cycles for the process
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		// endSem->release(); // Signal that the core has finished processing
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
	lock_guard<mutex> lock(mtx);
	return this->running.load();
}

void CPUCoreWorker::stopCoreThread()
{
	this->coreThreadRunning.store(false);
	if (this->coreThread.joinable()) {
		this->coreThread.join(); 
	}
}

shared_ptr<class Process> CPUCoreWorker::getCurrentProcess() const
{
	return this->currentProcess;
}

bool CPUCoreWorker::shouldInterrupt() const
{
	if (this->running.load() && this->currentQuantumCycles.load() == 0) {
		return true; // Interrupt if not running, quantum cycles are zero, or no current process
	}
	return false;
}

void CPUCoreWorker::setProcessBackToReadyState()
{

	this->currentProcess->setProcessState(Process::READY); // Set the process state to ready

}


