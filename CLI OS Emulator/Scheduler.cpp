#include "Scheduler.h"
#include "ProcessConsole.h"
#include "ConsoleManager.h"
#include <string>
#include <ctime>
#include <iostream>

using namespace std;

Scheduler::Scheduler(SchedulingAlgorithm algorithm, int numberOfCores, int quantumCycles, int batchProcessFreq, int minIns, int maxIns, int delaysPerExecution) : numberOfCores(4), // Assuming a fixed number of cores for simplicity
	algorithm(algorithm),
	quantumCycles(quantumCycles),
	batchProcessFreq(batchProcessFreq),
	minIns(minIns),
	maxIns(maxIns),
	delayPerExecution(delaysPerExecution)
{
	for (int i = 0; i < numberOfCores; i++) 
	{
		cores.push_back(make_unique<CPUCoreWorker>(i, quantumCycles, batchProcessFreq, minIns, maxIns, delaysPerExecution));
	}

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

		readyQueue.push(process);
	}
}

void Scheduler::addProcessToReadyQueue(shared_ptr<class Process> process)
{
	this->readyQueue.push(process);
}

void Scheduler::run()
{
	this->running.store(true); // Set running to true when starting the scheduler
	switch (this->algorithm)
	{
		case SchedulingAlgorithm::FCFS:
			this->schedulerThread = thread(&Scheduler::fcfs, this);
			this->schedulerThread.detach();
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
}

void Scheduler::fcfs()
{
	while (this->running.load())
	{
		for (int i = 0; i < this->numberOfCores; i++)
		{
			lock_guard<mutex> lock(mtx);
			// If the core is not running a process, assign a process from the ready queue
			if (!this->readyQueue.empty() && !cores[i]->isRunning())
			{
				shared_ptr<Process> process = this->readyQueue.front(); // get the first process in the queue
				this->readyQueue.pop(); // remove from the first process in the queue
				if (process != nullptr)
				{
					this->runningProcesses.push_back(process);
					this->cores[i]->doProcess(process);
				}
			}

		}

		// Check for finished processes and move them to finishedProcesses vector
		for (auto runningProcess = this->runningProcesses.begin(); runningProcess != this->runningProcesses.end();)
		{
			lock_guard<mutex> lock(mtx);
			if ((*runningProcess)->getProcessState() == Process::FINISHED)
			{
				this->finishedProcesses.push_back(*runningProcess);
				runningProcess = this->runningProcesses.erase(runningProcess); // remove finished process from running processes
			}
			else
			{
				++runningProcess; // move to the next process
			}
		}
	}
}

void Scheduler::printProcessesStatus()
{
	int runningCores = 0;

	for (int i = 0; i < numberOfCores; i++)
	{
		if (cores[i]->isRunning()) 
		{
			runningCores++;
		}
	}

	cout << "CPU Utilization: " << (runningCores * 100) / numberOfCores << "%\n";
	cout << "Cores used: " << runningCores << "\n";
	cout << "Cores available: " << numberOfCores - runningCores << "\n\n";
	cout << "--------------------------\n";

    cout << "Running processes:\n";
    for (const auto& process : runningProcesses) {
        int currentInstruction = process->getCurrentLine();
        int totalInstructions = process->getTotalLines();
        int CPUCoreID = process->getCPUCoreID();
        string processName = process->getName();
        time_t createdTime = process->getCreatedTime();

        tm now;
        localtime_s(&now, &createdTime);

        std::cout << left << setw(12) << processName                        
            << " (" << put_time(&now, "%m/%d/%Y %I:%M:%S%p") << ")    "     
            << std::setw(6) << "Core:" << std::setw(0) << CPUCoreID << "   "
            << currentInstruction << "/" << totalInstructions << "\n";         
    }

    cout << "\nFinished processes:\n";
    for (const auto& process : finishedProcesses) {
		int currentInstruction = process->getCurrentLine();
		int totalInstructions = process->getTotalLines();
		int CPUCoreID = process->getCPUCoreID();
		string processName = process->getName();
		time_t createdTime = process->getCreatedTime();

        tm now;
        localtime_s(&now, &createdTime);

        cout << std::left << setw(12) << processName                     
            << " (" << put_time(&now, "%m/%d/%Y %I:%M:%S%p") << ")    "     
            << std::setw(10) << "Finished"                                    
            << currentInstruction << "/" << totalInstructions << "\n";         

    }

    cout << "--------------------------\n";
}
