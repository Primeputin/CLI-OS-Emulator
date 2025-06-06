#pragma once
#include <string>
#include <ctime>
#include "ICommand.h"
#include <vector>
#include <memory>

using namespace std;

class Process
{
public:

	enum ProcessState
	{
		READY,
		RUNNING,
		WAITING,
		FINISHED
	};

	Process(int pid, string name, std::vector<std::shared_ptr<ICommand>> commandList);

	bool isFinished() const;
	int getRemainingLines() const;
	ProcessState getProcessState() const;
	void setProcessState(ProcessState newState);

	void addCommand(std::shared_ptr<ICommand> command);
	void executeCurrentCommand() const;
	void moveToNextLine();
	void setCPUCoreID(int coreID);

	// void test_generateRandomCommands(int count);

	string getName() const;
	uint16_t getCurrentLine() const;
	uint16_t getTotalLines() const;
	time_t getCreatedTime() const;
	int getCPUCoreID();

	private:
		int pid = -1; 
		string name;
		uint16_t currentLine;
		uint16_t totalLines;
		time_t createdTime;
		int cpuCoreID = -1; // -1 means not assigned to any CPU core
		typedef std::vector<std::shared_ptr<ICommand>> CommandList;
		CommandList commandList; // List of commands to be executed by the process
		ProcessState processState = READY; 
		
		
	
};

