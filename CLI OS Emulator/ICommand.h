#pragma once

#include <string>
class Process; 

class ICommand
{
	public:
		enum CommandType
		{
			DECLARE,
			PRINT,
			ADD,
			SUBTRACT,
			SLEEP,
			FORLOOP

		};

		ICommand(int pid, CommandType commandType);
		CommandType getCommandType() const;
		virtual void execute() = 0;
		virtual void logExecute(int cpuCoreID, std::string fileName) = 0;
		void setProcess(Process* p);
	protected:
		int pid;
		Process* process = nullptr;
		CommandType commandType;


};