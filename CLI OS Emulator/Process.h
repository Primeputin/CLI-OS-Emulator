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
		FINISHED
	};

	Process(int pid, string name, uint64_t totalLines, uint32_t memoryFrameSize, uint32_t minMemorySize, uint32_t maxMemorySize);

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
	bool isSleeping() const;

	string getName() const;
	uint64_t getCurrentLine() const;
	uint64_t getTotalLines() const;
	time_t getCreatedTime() const;
	int getCPUCoreID();
	void clearSymbolTable();

	void addLog(const std::string& logEntry);
	std::vector<std::string> getLogs() const;

	uint32_t getMemorySize();
	uint32_t getMinMemorySize();
	uint32_t getMaxMemorySize();
	uint32_t getMemoryFrameSize();
	uint32_t getNPages();
	void setRandomizedMemSize(uint32_t minMemorySize, uint32_t maxMemorySize);

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
	std::vector<std::string> logs;
	mutable std::mutex logMutex;
	uint32_t memorySize;
	uint32_t minMemorySize;
	uint32_t maxMemorySize;
	uint32_t memoryFrameSize;
	uint32_t nPages;
	time_t startOrEndTime;
};