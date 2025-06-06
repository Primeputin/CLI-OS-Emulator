#include "CPUCoreWorker.h"
#include <thread>

CPUCoreWorker::CPUCoreWorker(int coreID, int quantumCycles, int batchProcessFreq, int minIns, int maxIns, int delayPerExecution)
	: coreID(coreID), quantumCycles(quantumCycles), batchProcessFreq(batchProcessFreq),
	minIns(minIns), maxIns(maxIns), delayPerExecution(delayPerExecution), running(false)
{
}

void CPUCoreWorker::doProcess(std::shared_ptr<class Process> process)
{
	if (process == nullptr) {
		return; // Handle null process case
	}
	this->currentProcess = process;
	this->running = true; // Set running to true when starting the process
	
	process->setCPUCoreID(this->coreID); // Set the CPU core ID for the process
	process->setProcessState(Process::ProcessState::RUNNING);
	if (running && this->currentProcess)
	{
		thread(&CPUCoreWorker::run, this).detach(); // thread for running the process
	}
}

void CPUCoreWorker::run()
{
	while (this->currentProcess->getProcessState() != Process::FINISHED)
	{
		this_thread::sleep_for(std::chrono::milliseconds(delayPerExecution));
		
		lock_guard<mutex> lock(mtx);
		this->currentProcess->executeCurrentCommand(); // Execute the current command of the process
		this->currentProcess->moveToNextLine(); // Move to the next line after executing the command
		if (this->currentProcess->isFinished())
		{
			this->currentProcess->setProcessState(Process::FINISHED); // Set the process state to finished
			stop();
		}
		
	}
}

void CPUCoreWorker::stop()
{
	this->running = false; // Set running to false when stopping the process
}

int CPUCoreWorker::getCoreID() const
{
	return this->coreID;
}

bool CPUCoreWorker::isRunning() const
{
	return this->running;
}
