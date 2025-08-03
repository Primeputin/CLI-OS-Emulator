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

void MainConsole::parseProcessInstruction(string command, Process::CommandList& commandList)
{
    vector<string> texts = getInstructionParameters(command, { '(', ')' });

	/*for (size_t i = 0; i < texts.size(); i++) {
        cout << texts[i] << endl;
	}*/

	// cout << "[DEBUG] Parsing command: " << texts[0] << texts[1] << endl;

    if (texts[0] == "DECLARE") {
        commandList.push_back(make_shared<DeclareCommand>(-1, texts[1], stoi(texts[2]), nullptr));
    }
    else if (texts[0] == "PRINT") {

        if (texts.size() == 2) {
            if (texts[1] == "\"") {
                cout << texts[0] << " " << texts[1] << endl;
				string text = texts[1].substr(1, texts[1].length() - 2);
                commandList.push_back(make_shared<PrintCommand>(-1, text, nullptr));
            }
            else {
                commandList.push_back(make_shared<PrintVariableCommand>(-1, "", texts[2], nullptr));
            }
        }
        else if (texts.size() == 3) {
            string text = texts[1].substr(1, texts[1].length() - 2);
            commandList.push_back(make_shared<PrintVariableCommand>(-1, text, texts[2], nullptr));
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
            commandList.push_back(make_shared<WriteCommand>(-1, texts[1], texts[2], nullptr));
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

    // Extract command
    size_t startPos = instruction.find(groupingSymbol[0]);
    size_t endPos = instruction.find_last_of(groupingSymbol[1]);
    string command = instruction.substr(0, startPos);
    if (!command.empty() && command[0] == ' ') {
        command = command.substr(1);
    }

    // Extract everything between ( and )
    string parameters = instruction.substr(startPos + 1, endPos - startPos - 1);

    // Smart split by commas (ignoring commas inside quotes)
    vector<string> tokenizedParameters;
    bool inQuotes = false;
    string current;
    for (size_t i = 0; i < parameters.size(); ++i) {
        char c = parameters[i];

        if (c == '"' && (i == 0 || parameters[i - 1] != '\\')) {
            inQuotes = !inQuotes;  // toggle quote state
        }

        if (c == ',' && !inQuotes) {
            size_t s = current.find_first_not_of(" \t");
            size_t e = current.find_last_not_of(" \t");
            if (s != string::npos)
                tokenizedParameters.push_back(unescapeQuotes(current.substr(s, e - s + 1)));
            current.clear();
        }
        else {
            current += c;
        }
    }
    if (!current.empty()) {
        size_t s = current.find_first_not_of(" \t");
        size_t e = current.find_last_not_of(" \t");
        if (s != string::npos)
            tokenizedParameters.push_back(unescapeQuotes(current.substr(s, e - s + 1)));
    }

    // Add command and parameters
    parts.push_back(command);
    for (auto& param : tokenizedParameters) {
        parts.push_back(param);
    }

    return parts;
    
}

vector<string> MainConsole::parseInstructions(string instructions) {

    char delimiter = ';';

    long count = std::count(instructions.begin(), instructions.end(), delimiter);

    vector<string> parsedInstructions = tokenize(instructions, delimiter);

	//cout << "[DEBUG] Parsed instructions count: " << count << " | " << parsedInstructions.size() << endl;

	//cout << "[DEBUG] Parsed instructions: " << parsedInstructions[0] << parsedInstructions[0] << endl;

    if (parsedInstructions.size() != count) {
        cerr << "Error: Syntax Error." << endl;
        return {};
    }

    return parsedInstructions;
}

string MainConsole::unescapeQuotes(const string& s) {
    string result;
    result.reserve(s.size());
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size() && s[i + 1] == '"') {
            result.push_back('"');
            ++i; // skip the backslash
        }
        else {
            result.push_back(s[i]);
        }
    }
    return result;
}

string MainConsole::extractQuoted(const std::string& text) {
    bool inQuotes = false;
    std::string result;

    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '"' && (i == 0 || text[i - 1] != '\\')) {
            if (!inQuotes) {
                inQuotes = true; // start capturing
                continue;
            }
            else {
                break; // end capturing
            }
        }
        if (inQuotes) result += text[i];
    }

    return result;
}

void MainConsole::processCommand (string command)
{
    vector<string> texts = getSpacedTexts(command);

    string instructions = extractQuoted(command);

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