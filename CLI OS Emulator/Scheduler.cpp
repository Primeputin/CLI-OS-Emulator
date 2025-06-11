#include "Scheduler.h"
#include "ProcessConsole.h"
#include "ConsoleManager.h"
#include <string>
#include <ctime>
#include <iostream>

using namespace std;

Scheduler::Scheduler(SchedulingAlgorithm algorithm, int numberOfCores, uint64_t quantumCycles, uint64_t batchProcessFreq, uint64_t minIns, uint64_t maxIns, uint64_t delayPerExecution) : numberOfCores(numberOfCores),
	algorithm(algorithm),
	quantumCycles(quantumCycles),
	batchProcessFreq(batchProcessFreq),
	minIns(minIns),
	maxIns(maxIns),
	delayPerExecution(delayPerExecution)
{
	for (int i = 0; i < numberOfCores; i++) 
	{
		shared_ptr<binary_semaphore> newStartSem = make_shared<binary_semaphore>(0); // Create a semaphore for each core
		startSem.push_back(newStartSem); // Initialize semaphores for each core
		shared_ptr<binary_semaphore> newEndSem = make_shared<binary_semaphore>(0); // Create a semaphore for each core
		endSem.push_back(newEndSem); // Initialize semaphores for each core
		cores.push_back(make_unique<CPUCoreWorker>(i, quantumCycles, batchProcessFreq, minIns, maxIns, delayPerExecution, newStartSem, newEndSem));
	}
	
	/*
	for (int id = 0; id < 10; id++)
	{
		ConsoleManager* consoleManager = ConsoleManager::getInstance();

		string screenName = "process_" + to_string(id);

		vector<shared_ptr<ICommand>> commandList;
		for (int j = 0; j < 100; j++)
		{
			commandList.push_back(make_shared<PrintCommand>(id, "Hello World!"));
		}

		shared_ptr<Process> process = make_shared<Process>(id, screenName, commandList);

		shared_ptr<ProcessConsole> newConsole = make_shared<ProcessConsole>(process);

		consoleManager->addToConsoleTable(screenName, newConsole);

		addProcessToReadyQueue(process); // Add the process to the ready queue
	}*/
	
}

void Scheduler::addProcessToReadyQueue(shared_ptr<class Process> process)
{
	std::lock_guard<std::mutex> lock(queueMutex);
	this->readyQueue.push(process);
}

void Scheduler::assignProcessToCore(int coreID)
{
	std::shared_ptr<Process> process;

	// Lock only while accessing the queue
	{
		std::lock_guard<std::mutex> qLock(queueMutex);
		if (readyQueue.empty()) return;
		process = readyQueue.front();
		readyQueue.pop();
	}

	// Lock only while updating running processes
	{
		std::lock_guard<std::mutex> rLock(runningMutex);
		runningProcesses.push_back(process);
	    // cout << "Process " << process->getName() << " assigned to core " << coreID << " CPU tick:" << totalCycles.load() << endl;
	}

	cores[coreID]->doProcess(process);  // Core has its own mutex
}

void Scheduler::checkFinishedProcesses()
{
	std::lock_guard<std::mutex> rLock(runningMutex);
	std::lock_guard<std::mutex> fLock(finishedMutex);

	for (auto it = runningProcesses.begin(); it != runningProcesses.end(); ) {
		if ((*it)->getProcessState() == Process::FINISHED) {
			// cout << "Process " << (*it)->getName() << " finished on core " << (*it)->getCPUCoreID() << " CPU tick:" << totalCycles.load() << endl;
			finishedProcesses.push_back(*it);
			it = runningProcesses.erase(it);
		}
		else {
			++it;
		}
	}
}

void Scheduler::generateProcesses()
{
	this->generate.store(true); // Set generate to true when starting the process generation
}

void Scheduler::stopGenerationOfProcesses()
{
	this->generate.store(false); // Set generate to false when stopping the process generation
}

shared_ptr<Console> Scheduler::generateRandomProcess(string name)
{

	// Generate a process
	ConsoleManager* consoleManager = ConsoleManager::getInstance();

	if (consoleManager->consoleExists(name)) {
		std::cerr << "Process with name " << name << " already exists. Please use a different name." << std::endl;
		return nullptr; // Return nullptr if the process already exists
	}


	uint64_t id = getTotalProcesses();

	vector<shared_ptr<ICommand>> commandList;
	for (int j = 0; j < 100; j++)
	{
		commandList.push_back(make_shared<PrintCommand>(id, "Hello world from " + name + " !"));
	}

	shared_ptr<Process> process = make_shared<Process>(id, name, commandList);

	shared_ptr<ProcessConsole> newConsole = make_shared<ProcessConsole>(process);

	consoleManager->addToConsoleTable(name, newConsole);
	addProcessToReadyQueue(process); // Add the process to the ready queue
	totalProcesses++;
	return newConsole; // Return the new console for the process
	
}

void Scheduler::run()
{
	this->running.store(true); // Set running to true when starting the scheduler
	switch (this->algorithm)
	{
		case SchedulingAlgorithm::FCFS:
			this->schedulerThread = thread(&Scheduler::fcfs, this);
			break;
		case SchedulingAlgorithm::RR:
			// Implement Round Robin scheduling here
			break;
		default:
			std::cerr << "Unknown scheduling algorithm." << std::endl;
			break;
	}
}

void Scheduler::stop()
{
	this->running.store(false);
	if (this->schedulerThread.joinable()) {
		this->schedulerThread.join(); // wait for the thread to finish
	}
}

void Scheduler::fcfs()
{
	uint32_t batchCycles = 0;
	while (this->running.load())
	{

		checkFinishedProcesses(); // Check for finished processes

		for (int i = 0; i < numberOfCores; i++) {
			if (!cores[i]->isRunning()) {
				assignProcessToCore(i);  
			}
		}
 
		// Tell all cpu core workers to start running an instruction line
		for (int i = 0; i < startSem.size(); i++) {
			startSem[i]->release();
		}
		for (int i = 0; i < endSem.size(); i++) {
			endSem[i]->acquire();
		}

		batchCycles++;
		if (batchCycles >= batchProcessFreq)
		{
			if (generate.load())
			{
				string name = "process_" + to_string(latestProcessID.load());
				while (ConsoleManager::getInstance()->consoleExists(name)) {
					latestProcessID++;
					name = "process_" + to_string(latestProcessID.load());
				}
				generateRandomProcess(name);
			}
			batchCycles = 0; // Reset batch cycles after adding a new process
		}
		
		totalCycles++;
	}
}

void Scheduler::printProcessesStatus(std::ostream& out)
{
	int runningCores = 0;

	for (int i = 0; i < numberOfCores; i++) {
		if (cores[i]->isRunning()) {
			runningCores++;
		}
	}

	out << "CPU Utilization: " << (runningCores * 100) / numberOfCores << "%\n";
	out << "Cores used: " << runningCores << "\n";
	out << "Cores available: " << numberOfCores - runningCores << "\n\n";
	out << "--------------------------\n";

	std::lock_guard<std::mutex> rLock(runningMutex);
	std::lock_guard<std::mutex> fLock(finishedMutex);

	out << "Running processes:\n";
	for (const auto& process : runningProcesses) {
		int currentInstruction = process->getCurrentLine();
		int totalInstructions = process->getTotalLines();
		int CPUCoreID = process->getCPUCoreID();
		std::string processName = process->getName();
		time_t createdTime = process->getCreatedTime();

		tm now;
		localtime_s(&now, &createdTime);

		out << std::left << std::setw(12) << processName
			<< " (" << std::put_time(&now, "%m/%d/%Y %I:%M:%S%p") << ")    "
			<< std::setw(6) << "Core:" << std::setw(0) << CPUCoreID << "   "
			<< currentInstruction << "/" << totalInstructions << "\n";
	}

	out << "\nFinished processes:\n";
	for (const auto& process : finishedProcesses) {
		int currentInstruction = process->getCurrentLine();
		int totalInstructions = process->getTotalLines();
		std::string processName = process->getName();
		time_t createdTime = process->getCreatedTime();

		tm now;
		localtime_s(&now, &createdTime);

		out << std::left << std::setw(12) << processName
			<< " (" << std::put_time(&now, "%m/%d/%Y %I:%M:%S%p") << ")    "
			<< std::setw(10) << "Finished"
			<< currentInstruction << "/" << totalInstructions << "\n";
	}

	out << "--------------------------\n";
}


uint64_t Scheduler::getTotalCycles()
{
	return this->totalCycles;
}

uint64_t Scheduler::getTotalProcesses()
{

	return this->totalProcesses;
}
