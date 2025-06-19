#pragma once

#include <unordered_map>
#include <string>
#include <ctime>
#include "ICommand.h"
#include <vector>
#include <memory>
#include <mutex>	

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

	Process(int pid, string name, uint64_t totalLines);

	int getPID() const;
	bool isFinished() const;
	uint64_t getRemainingLines() const;
	ProcessState getProcessState() const;
	void setProcessState(ProcessState newState);

	void addCommand(std::shared_ptr<ICommand> command);
	void executeCurrentCommand() const;
	void moveToNextLine();
	void setCPUCoreID(int coreID);

	void declareVariable(const std::string& varName, uint16_t value);
	bool getVariableValue(const std::string& varName, uint16_t& outValue) const;
	void decrementSleepTick();
	bool isSleeping() const;

	string getName() const;
	uint64_t getCurrentLine() const;
	uint64_t getTotalLines() const;
	time_t getCreatedTime() const;
	int getCPUCoreID();
	void clearSymbolTable();

	private:
		int pid = -1; 
		string name;
		uint64_t currentLine;
		uint64_t totalLines;
		time_t createdTime;
		int cpuCoreID = -1; // -1 means not assigned to any CPU core
		typedef std::vector<std::shared_ptr<ICommand>> CommandList;
		CommandList commandList; // List of commands to be executed by the process
		ProcessState processState = READY; 
		unordered_map<string, uint16_t> symbolTable;
		mutable std::mutex mtx; // Mutex for thread safety when accessing process state and commands
		mutable std::mutex varAccess; // Mutex for symbol table access
		void generateCommands();
	
};

