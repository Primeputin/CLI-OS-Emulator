#include "MainConsole.h"
#include "ProcessConsole.h"
#include "ConsoleManager.h"
#include "Console.h"
#include "Process.h"
#include <iostream>
#include "PrintCommand.h"
#include "PrintVariableCommand.h"
#include "DeclareCommand.h"
#include "AddCommand.h"
#include "SubtractCommand.h"
#include "SleepCommand.h"
#include "DemandPagingMemoryManager.h"
#include "ReadCommand.h"
#include "WriteCommand.h"
#include <regex>

// ANSI escape codes for color
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define RESET   "\033[0m"

MainConsole::MainConsole() : Console("MAIN_CONSOLE") {
	
}

void MainConsole::header() {

    std::cout << R"(  
  ______    ______    ______   _______   ________   ______   __      __ 
 /      \  /      \  /      \ /       \ /        | /      \ /  \    /  |
/$$$$$$  |/$$$$$$  |/$$$$$$  |$$$$$$$  |$$$$$$$$/ /$$$$$$  |$$  \  /$$/ 
$$ |  $$/ $$ \__$$/ $$ |  $$ |$$ |__$$ |$$ |__    $$ \__$$/  $$  \/$$/  
$$ |      $$      \ $$ |  $$ |$$    $$/ $$    |   $$      \   $$  $$/   
$$ |   __  $$$$$$  |$$ |  $$ |$$$$$$$/  $$$$$/     $$$$$$  |   $$$$/    
$$ \__/  |/  \__$$ |$$ \__$$ |$$ |      $$ |_____ /  \__$$ |    $$ |    
$$    $$/ $$    $$/ $$    $$/ $$ |      $$       |$$    $$/     $$ |    
 $$$$$$/   $$$$$$/   $$$$$$/  $$/       $$$$$$$$/  $$$$$$/      $$/     )" << '\n';


    std::cout << '\n';

    std::cout << GREEN << "Hello, Welcome to CSOPESY commandline!" << RESET << '\n';
    std::cout << YELLOW << "Type 'exit' to quit, 'clear' to clear the screen" << RESET << '\n';
    std::cout << '\n';
}

void MainConsole::recognizedCommand(string command)
{
    cout << command << " recognized, Doing something." << endl;
}

void MainConsole::initialize(string command)
{
    recognizedCommand(command);
	if (!ConsoleManager::getInstance()->isSchedulerInitialized())
	{
        ConsoleManager::getInstance()->initScheduler();
        ConsoleManager::getInstance()->runScheduler();
	}
    else
    {
        ConsoleManager::getInstance()->initScheduler();
    }
    
}

void MainConsole::screen(string command)
{
    recognizedCommand(command);
}

void MainConsole::schedulerStart(string command)
{
    recognizedCommand(command);
    ConsoleManager::getInstance()->generateProcesses();
}

void MainConsole::schedulerStop(string command)
{
    recognizedCommand(command);
    ConsoleManager::getInstance()->stopGenerationOfProcesses();
}

void MainConsole::reportUtil(string command)
{
    recognizedCommand(command);
	ConsoleManager::getInstance()->listProcesses(true);
}

void MainConsole::vmstat(string command)
{
    recognizedCommand(command);
    ConsoleManager::getInstance()->vmstat();
}

void MainConsole::processSmi(string command)
{
    recognizedCommand(command);
	ConsoleManager::getInstance()->processSmi();
}

void MainConsole::createConfiguredProcess(string name, uint16_t memorySize, string command)
{
    recognizedCommand(command);
    ConsoleManager::getInstance()->createConfiguredProcess(name, memorySize, addInstructions(command));
}


Process::CommandList MainConsole::addInstructions(string instructions) {
    if (instructions.empty()) {
        cout << "No instructions provided." << endl;
        return {};
    }

    vector<string> parsedInstructions = parseInstructions(instructions);
    Process::CommandList commandList;

    if (parsedInstructions.empty()) {
        return {};
    }
    else {
        for (int i = 0; i < parsedInstructions.size(); i++) {
            parseProcessInstruction(parsedInstructions[i], commandList);
        }
    }
    return commandList;
}

void MainConsole::parseProcessInstruction(string command, Process::CommandList commandList)
{
    vector<string> texts = getInstructionParameters(command, { '(', ')' });
	for (size_t i = 0; i < texts.size(); i++) {
        cout << texts[i] << endl;
	}
    if (texts[0] == "DECLARE") {
        commandList.push_back(make_shared<DeclareCommand>(-1, texts[1], stoi(texts[2]), nullptr));
    }
    else if (texts[0] == "PRINT") {

        //TODO

        if (texts.size() == 2) {
            commandList.push_back(make_shared<PrintCommand>(-1, texts[1], nullptr));
        }
        else {
            cerr << "Error: Invalid PRINT command syntax." << endl;
        }
    }
    else if (texts[0] == "ADD") {
        if (texts.size() == 4) {
            commandList.push_back(make_shared<AddCommand>(-1, texts[1], texts[2], texts[3], nullptr));
        }
        else {
            cerr << "Error: Invalid ADD command syntax." << endl;
        }
    }
    else if (texts[0] == "SUBTRACT") {
        if (texts.size() == 4) {
            commandList.push_back(make_shared<SubtractCommand>(-1, texts[1], texts[2], texts[3], nullptr));
        }
        else {
            cerr << "Error: Invalid SUBTRACT command syntax." << endl;
        }
    }
    else if (texts[0] == "SLEEP") {
        if (texts.size() == 2) {
            commandList.push_back(make_shared<SleepCommand>(-1, stoi(texts[1]), nullptr));
        }
        else {
            cerr << "Error: Invalid SLEEP command syntax." << endl;
        }
    }
    else if (texts[0] == "READ") {
        if (texts.size() == 3) {
            commandList.push_back(make_shared<ReadCommand>(-1, texts[1], texts[2], nullptr));
        }
        else {
            cerr << "Error: Invalid READ command syntax." << endl;
        }
    }
    else if (texts[0] == "WRITE") {
        if (texts.size() == 3) {
            commandList.push_back(make_shared<WriteCommand>(-1, texts[1], stoi(texts[2]), nullptr));
        }
        else {
            cerr << "Error: Invalid WRITE command syntax." << endl;
        }
    }
    else if (texts[0] == "FOR") {

        vector<string> loopParts = getInstructionParameters(texts[1], { '[', ']' });

        for (int i = 0; i < stoi(texts[2]); i++) {
            for (size_t j = 2; j < texts.size(); j++) {
                parseProcessInstruction(texts[j], commandList);
            }
        }
    }
    else {
        cerr << "Error: Unknown command '" << texts[0] << "'." << endl;
    }
}

vector<string> MainConsole::tokenize(string str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string token;
    while (getline(ss, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

vector<string> MainConsole::getInstructionParameters(string instruction, vector<char> groupingSymbol) {

    vector<string> parts;
    string command = instruction.substr(0, instruction.find(groupingSymbol[0]));

    if(command[0] == ' ') {
        command = command.substr(1); 
	}

    string parameters = instruction.substr(instruction.find(groupingSymbol[0]) + 1, instruction.find(groupingSymbol[1]));
    if (parameters.back() == groupingSymbol[1]) {
        parameters.pop_back(); 
	}
    vector<string> tokenizedParameters = tokenize(parameters, ' ');


    parts.push_back(command); // Add the command itself
    for (string param : tokenizedParameters) {
        if (!param.empty()) {
            param = param.substr(0, param.find(','));
            parts.push_back(param); // Add each parameter
        }
    }

    return parts;
}

vector<string> MainConsole::parseInstructions(string instructions) {

    char delimiter = ';';

    long count = std::count(instructions.begin(), instructions.end(), delimiter);

    vector<string> parsedInstructions = tokenize(instructions, delimiter);

    if (parsedInstructions.size() != count) {
        cerr << "Error: Syntax Error." << endl;
        return {};
    }

    return parsedInstructions;
}

void MainConsole::processCommand (string command)
{
    vector<string> texts = getSpacedTexts(command);
    
    std::regex re("\"(.*?)\"");  // Match text between first pair of double quotes
    std::smatch match;

    string instructions = "";

    if (std::regex_search(command, match, re)) {
        instructions = match[1].str();
    }


    if (!ConsoleManager::getInstance()->isSchedulerInitialized())
    {
        if (command == "initialize")
        {
            initialize(command);
        }
        else if (command == "exit")
        {
            recognizedCommand(command);
            ConsoleManager::getInstance()->stop();
        }
        else
        {
			cout << "You entered: " << command << "\n" << "Command invalid, please initialize the scheduler first." << "\n\n";
        }
    }
    else if (texts.size() > 4 && texts[0] == "screen" && texts[1] == "-c" && instructions != "")
    {

        uint16_t val;
        if (!parse_uint16_within_range(texts[3], val)) {
            cout << "A process may contain 2^6 - 2^16 bytes of memory only" << "\n\n";
        }
        else
        {
            recognizedCommand(command);
            createConfiguredProcess(texts[2], val, instructions);
        }

    }
    else if (texts.size() == 1)
    {
        if (command == "initialize")
        {
            initialize(command);
        }
        else if (command == "screen")
        {
            screen(command);
        }
        else if (command == "scheduler-start")
        {
            schedulerStart(command);
        }
        else if (command == "scheduler-stop")
        {
            schedulerStop(command);
        }
        else if (command == "report-util")
        {
            reportUtil(command);
        }
        else if (command == "vmstat")
        {
            vmstat(command);
        }
        else if (command == "process-smi")
        {
			processSmi(command);
        }
        else if (command == "exit")
        {
            recognizedCommand(command);
			ConsoleManager::getInstance()->stop();
        }
        else if (command == "clear") {
            recognizedCommand(command);
            Console::clear();
            header();
        }
        else
        {
            cout << "You entered: " << command << "\n" << "Command invalid" << "\n\n";
        }
    }
	else if (texts.size() == 2 && texts[0] == "screen" && texts[1] == "-ls")
	{
        recognizedCommand(command);
        ConsoleManager::getInstance()->listProcesses(false);
	}
	else if (texts.size() == 3 && texts[0] == "screen")
	{
		if (texts[1] == "-r")
		{
			recognizedCommand(command);
            ConsoleManager::getInstance()->switchToProcessConsole(texts[2]);
		}
        else
        {
            cout << "You entered: " << command << "\n" << "Command invalid" << "\n\n";
        }
	}
    else if (texts.size() == 4 && texts[0] == "screen" && texts[1] == "-s")
    {
        
        uint16_t val;
        if (!parse_uint16_within_range(texts[3], val)) {
            cout << "A process may contain 2^6 - 2^16 bytes of memory only" << "\n\n";
        }
        else
        {
            recognizedCommand(command);
            ConsoleManager::getInstance()->createProcess(texts[2], val);
        }
    }
    else if (texts.size() != 0){
        cout << "You entered: " << command << "\n" << "Command invalid" << "\n\n";
    }
}

void MainConsole::getCommand()
{
    string command;
    cout << "Enter a command: ";
    getline(cin, command);
    processCommand(command);
}

bool MainConsole::parse_uint16_within_range(const std::string& input, uint16_t& out) {
    if (!input.empty() && input[0] == '-')
        return false;

    try {
        unsigned long val = std::stoul(input);
        if (val < 64 || val > std::numeric_limits<uint16_t>::max())
            return false;
        out = static_cast<uint16_t>(val); // modify the value even outside this function
        return true;
    }
    catch (...) {
        return false;
    }
}