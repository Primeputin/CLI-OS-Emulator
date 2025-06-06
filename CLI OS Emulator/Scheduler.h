#pragma once

#include <string>
#include <memory>
#include <vector>
#include <queue>
#include <ctime>
#include <thread>
#include <atomic>
#include <mutex>
#include "CPUCoreWorker.h"
#include "Process.h"


using namespace std;

class Scheduler
{
	public:
		enum SchedulingAlgorithm
		{
			FCFS, // First Come First Serve
			RR,   // Round Robin
		};
		Scheduler(SchedulingAlgorithm algorithm, int numberOfCores, int quantumCycles, int batchProcessFreq, int minIns, int maxIns, int delaysPerExecution);

		void addProcessToReadyQueue(shared_ptr<class Process> process);
		void run();
		void stop();
		void fcfs();
		void printProcessesStatus();
	private:
		int numberOfCores;
		SchedulingAlgorithm algorithm;
		int quantumCycles;
		int batchProcessFreq;
		int minIns;
		int maxIns;
		int delayPerExecution;
		thread schedulerThread;
		atomic<bool> running = false;
		mutex mtx;

		vector<unique_ptr<CPUCoreWorker>> cores;
		queue<shared_ptr<Process>> readyQueue;
		vector<shared_ptr<Process>> runningProcesses;
		vector<shared_ptr<Process>> finishedProcesses;
};

