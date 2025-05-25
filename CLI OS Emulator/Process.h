#pragma once
#include <string>
#include <ctime>

using namespace std;

class Process
{
	private:
		//uint32_t id; for future implementation
		string name;
		uint16_t currentLine;
		uint16_t totalLines;
		time_t createdTime;
		
	public:
		Process(string name, uint16_t currentLine, uint16_t totalLines);
		string getName() const;
		uint16_t getCurrentLine() const;
		uint16_t getTotalLines() const;
		time_t getCreatedTime() const;
};

