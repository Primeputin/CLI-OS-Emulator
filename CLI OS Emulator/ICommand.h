#pragma once
#include <string>

class ICommand
{
	public:
		enum CommandType
		{
			IO,
			PRINT,
		};

		ICommand(int pid, CommandType commandType);
		CommandType getCommandType() const;
		virtual void execute() = 0;
		virtual void logExecute(int cpuCoreID, std::string fileName) const = 0;
	protected:
		int pid;
		CommandType commandType;


};