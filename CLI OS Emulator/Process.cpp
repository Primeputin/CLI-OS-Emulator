#include "Process.h"
#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <functional>
#include "PrintCommand.h"
#include "PrintVariableCommand.h"
#include "DeclareCommand.h"
#include "AddCommand.h"
#include "SubtractCommand.h"
#include "SleepCommand.h"


Process::Process(int pid, string name, uint64_t totalLines)
{
	this->pid = pid;
	this->name = name;
	this->currentLine = 0;
	this->totalLines = totalLines;
	this->createdTime = time(nullptr);

	generateCommands();
	// Example commands for testing purposes
	/*symbolTable["x"] = 0;
	symbolTable["y"] = 0;
	symbolTable["y"] = 0;
	for (int i = 0; i < 100; i++)
	{
		this->addCommand(make_shared<AddCommand>(pid, "x", "x", "1", this));
		this->addCommand(make_shared<PrintVariableCommand>(pid, "x", this));
		this->addCommand(make_shared<AddCommand>(pid, "y", "y", "1", this));
		this->addCommand(make_shared<PrintVariableCommand>(pid, "y", this));
		this->addCommand(make_shared<AddCommand>(pid, "z", "z", "1", this));
		this->addCommand(make_shared<PrintVariableCommand>(pid, "z", this));
	}*/
	
}

int Process::getPID() const
{
	return this->pid;
}

bool Process::isFinished() const {
	lock_guard<mutex> lock(mtx);
	return this->currentLine >= totalLines;
}

uint64_t Process::getRemainingLines() const {
	lock_guard<mutex> lock(mtx);
	return totalLines - currentLine;
}

Process::ProcessState Process::getProcessState() const {
	lock_guard<std::mutex> lock(mtx); // Lock the mutex to ensure thread safety
	return this->processState;
}

void Process::setProcessState(ProcessState newState)
{
	lock_guard<std::mutex> lock(mtx); // Lock the mutex to ensure thread safety
	this->processState = newState;
}

void Process::addCommand(std::shared_ptr<ICommand> command) {
	commandList.push_back(command);
}

void Process::executeCurrentCommand() const {
	// lock_guard<mutex> lock(mtx);
	if (!commandList.empty() && currentLine < commandList.size()) {
		// this->commandList[this->currentLine]->execute(); // use this if you don't want to log it on a text file
		this->commandList[this->currentLine]->logExecute(cpuCoreID, name);
	}
}

void Process::moveToNextLine() {
	lock_guard<mutex> lock(mtx);
	if (currentLine < totalLines) {
		currentLine++;
	}
}

void Process::setCPUCoreID(int coreID)
{
	this->cpuCoreID = coreID;
}

void Process::declareVariable(const std::string& varName, uint16_t value)
{
	std::lock_guard<std::mutex> symLock(varAccess);
	symbolTable[varName] = value;
}

bool Process::getVariableValue(const std::string& varName, uint16_t& outValue) const
{
	std::lock_guard<std::mutex> symLock(varAccess);
	auto it = symbolTable.find(varName);
	if (it != symbolTable.end()) {
		outValue = it->second;
		return true;
	}
	return false;
}

bool Process::isSleeping() const
{
	if (!commandList.empty() && getCurrentLine() < commandList.size()) {
		auto sleepCommand = dynamic_cast<SleepCommand*>(commandList[getCurrentLine()].get());
		if (sleepCommand) {
			return sleepCommand->isSleeping();
		}
	}
	return false;
}

string Process::getName() const {
    return name;
}

uint64_t Process::getCurrentLine() const {
    return currentLine;
}

uint64_t Process::getTotalLines() const {
    return totalLines;
}

time_t Process::getCreatedTime() const {
	return createdTime;
}

int Process::getCPUCoreID()
{
	return cpuCoreID;
}
void Process::addLog(const std::string& logEntry) {
	std::lock_guard<std::mutex> lock(logMutex);
	logs.push_back(logEntry);
}

std::vector<std::string> Process::getLogs() const {
	std::lock_guard<std::mutex> lock(logMutex);
	return logs;
}

void Process::clearSymbolTable() {
	symbolTable.clear();
}

void Process::generateCommands() {
	uint64_t numberOfVariables = 0;
	bool isResultDeclared = false;
	int commandsGenerated = 0;
	const int MAX_NEST_LEVEL = 3;

	// Helper lambda that contains the original randomization logic for basic commands
	auto generateSingleCommand = [&]() -> shared_ptr<ICommand> {
		int randomizedCommand = rand() % 5; // Randomly choose a command type

		switch (randomizedCommand)
		{
			case 0: // Declare command
			{
				string varName = "var_" + to_string(numberOfVariables);
				uint16_t defaultValue = 1; // Random value for the variable
				numberOfVariables++;
				return make_shared<DeclareCommand>(pid, varName, defaultValue, this);
				break;
			}
			case 1: // Print command
			{
				return make_shared<PrintCommand>(pid, "Hello world from " + name + "!", this);
				break;
			}
			case 2:
			{
				int numberOfVariations = 4;
				if (numberOfVariables == 0) // If no variables declared, skip to next iteration
				{
					numberOfVariations = 2;
				}
				int randomizedVariations = rand() % numberOfVariations;

				switch (randomizedVariations)
				{

				case 0: // both operands are numbers
				{
					isResultDeclared = true;
					return make_shared<AddCommand>(pid, "result_var", "50", "100", this); 
					break;
				}
				case 1: // one of the operand variables isn't declared yet
				{
					isResultDeclared = true;
					return make_shared<AddCommand>(pid, "result_var", "thisVarDoesntExist", "100", this);
					break;
				}
				case 2: // operands are variables
				{
					isResultDeclared = true;
					return make_shared<AddCommand>(pid, to_string(rand() % numberOfVariables), "var_" + to_string(rand() % numberOfVariables), "var_" + to_string(rand() % numberOfVariables), this);
					break;
				}
				case 3: // one operand is a variable, the other is a number
				{
					isResultDeclared = true;
					return make_shared<AddCommand>(pid, to_string(rand() % numberOfVariables), "50", "var_" + to_string(rand() % numberOfVariables), this);
					break;
				}

				}
				break;

			}
			case 3:
			{
				int numberOfVariations = 4;
				if (numberOfVariables == 0) // If no variables declared, skip to next iteration
				{
					numberOfVariations = 2;
				}
				int randomizedVariations = rand() % numberOfVariations;

				switch (randomizedVariations)
				{

				case 0: // both operands are numbers
				{
					isResultDeclared = true;
					return make_shared<SubtractCommand>(pid, "result_var", "50", "100", this);
					break;
				}
				case 1: // one of the operand variables isn't declared yet
				{
					isResultDeclared = true;
					return make_shared<SubtractCommand>(pid, "result_var", "thisVarDoesntExist", "100", this);
					break;
				}
				case 2: // operands are variables
				{
					isResultDeclared = true;
					return make_shared<SubtractCommand>(pid, to_string(rand() % numberOfVariables), "var_" + to_string(rand() % numberOfVariables), "var_" + to_string(rand() % numberOfVariables), this);
					break;
				}
				case 3: // one operand is a variable, the other is a number
				{
					isResultDeclared = true;
					return make_shared<SubtractCommand>(pid, to_string(rand() % numberOfVariables), "50", "var_" + to_string(rand() % numberOfVariables), this);
					break;
				}

				}
				break;
			}
			case 4: // Sleep command
			{
				return make_shared<SleepCommand>(pid, rand() % 256, this);
				break;
			}
			case 5: // Print command for variable value
			{
				int numberOfVariations = 1;
				if (isResultDeclared) // If result variable is declared, print it
				{
					numberOfVariations = 2;
				}

				if (numberOfVariables > 0)
				{
					int randomizedType = rand() % numberOfVariations; // Randomly choose a type of print variable command

					if (randomizedType == 0)
					{
						int randomizedVariables = rand() % numberOfVariables; // Randomly choose a variable to print
						string varName = "var_" + to_string(randomizedVariables);
						return make_shared<PrintVariableCommand>(pid, varName, this);
					}
					else
					{
						return make_shared<PrintVariableCommand>(pid, "result_var", this); // Print the result variable
					}
				}
				else
				{
					int randomizedType = rand() % numberOfVariations; // Randomly choose a type of print variable command
					if (randomizedType == 0)
					{
						return make_shared<PrintCommand>(pid, "Hello world from " + name + "!", this);
					}
					else
					{
						return make_shared<PrintVariableCommand>(pid, "result_var", this); // Print the result variable
					}

				}
				break;
			}
			default:
			{
				return nullptr; // Should not happen, but just in case
				break;
			}
		}
	};

	// Recursive command sequence generator
	function<vector<shared_ptr<ICommand>>(int, int, int&)> generateCommandSequence;
	generateCommandSequence = [&](int maxCommands, int currentDepth, int& generatedCount) -> vector<shared_ptr<ICommand>> {
		vector<shared_ptr<ICommand>> sequence;
		int localGenerated = 0;

		while (localGenerated < maxCommands) {
			int remaining = maxCommands - localGenerated;
			int commandChoice = rand() % 7;

			// Handle loop case (if allowed by depth and remaining commands)
			if (commandChoice == 6 && currentDepth < MAX_NEST_LEVEL && remaining > 1) {
				// Calculate iterations and body size based on remaining commands
				int maxIterations = min(3, remaining);
				int iterations = 1 + rand() % maxIterations;
				int maxBodySize = remaining / iterations;
				int bodySize = 1 + rand() % maxBodySize;

				/*cout << "Depth: " << currentDepth << " " << iterations << endl;
				cout << "Depth: " << currentDepth << " " << bodySize << endl;*/

				// Generate loop body recursively
				int bodyGenerated = 0;
				vector<shared_ptr<ICommand>> loopBody =
					generateCommandSequence(bodySize, currentDepth + 1, bodyGenerated);
				// Repeat body N times
				for (int i = 0; i < iterations && localGenerated < maxCommands; i++) {
					for (auto& cmd : loopBody) {
						if (localGenerated >= maxCommands) break;
						sequence.push_back(cmd);
						localGenerated++;
					}
				}
			}
			// Handle basic commands
			else {
				// cout << "Depth: " << currentDepth << " Not for loop" << endl;
				sequence.push_back(generateSingleCommand());
				localGenerated++;
			}
		}

		generatedCount = localGenerated;
		return sequence;
		};

	// Generate the entire command sequence
	int tempCounter = 0;
	commandList = generateCommandSequence(totalLines, 0, tempCounter);

}