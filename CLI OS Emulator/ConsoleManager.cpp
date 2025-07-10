#include "ConsoleManager.h"
#include "MainConsole.h"
#include "ProcessConsole.h"
#include "Console.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <memory>

namespace fs = std::filesystem;

const string MAIN_CONSOLE_NAME = "MAIN_CONSOLE";

ConsoleManager* ConsoleManager::sharedInstance = nullptr;
ConsoleManager* ConsoleManager::getInstance()
{
	return sharedInstance;
}

void ConsoleManager::initialize()
{
	if (sharedInstance) {
		std::cerr << "ConsoleManager is already initialized." << std::endl;
		return;
	}
	string folderPath = "output";

	// for making an output folder
	if (fs::exists(folderPath)) {
		fs::remove_all(folderPath);  // Deletes the folder and all contents
	}
	fs::create_directory(folderPath); // Recreates the empty folder

	sharedInstance = new ConsoleManager();
}

void ConsoleManager::destroy()
{
	if (sharedInstance) {
		delete sharedInstance;
		sharedInstance = nullptr;
	}
}

ConsoleManager::ConsoleManager() {
	this->running = true; // Set running to true when starting the console manager
	shared_ptr<MainConsole> mainConsole = make_shared<MainConsole>();
	consoleTable.insert({ mainConsole->getName(), mainConsole });
	this->currentConsole = mainConsole;
	this->currentConsole->header(); // Call the header function of the current console
	switchToMain(); // Switch to the main console
}

void ConsoleManager::createProcess(string name)
{

	shared_ptr<Console> newConsole = this->scheduler->generateRandomProcess(name);

	if (newConsole)
	{

		cout << "Screen name: " << name << " created successfully." << std::endl;
		//switchToProcessConsole(newConsole->getName());
	}
}

void ConsoleManager::destroyProcess(string name) {
	this->consoleTable.erase(name);
}

void ConsoleManager::switchToMain()
{
	Console::clear();
	this->currentConsole = this->consoleTable.at(MAIN_CONSOLE_NAME);
	this->currentConsole->header();
}

void ConsoleManager::switchToProcessConsole(string name)
{
	if (consoleTable.contains(name)) {
		Console::clear();
		this->currentConsole = this->consoleTable.at(name);
		this->currentConsole->header();
	}
	else {
		cerr << "None found" << endl;
	}
}

void ConsoleManager::addToConsoleTable(string name, shared_ptr<Console> console)
{
	if (consoleTable.find(name) == consoleTable.end()) {
		consoleTable.insert({ name, console });
	}
	else {
		cerr << "Console with name " << name << " already exists." << endl;
	}
}

bool ConsoleManager::consoleExists(string name) const
{
	if (consoleTable.find(name) != this->consoleTable.end()) {
		return true;
	}
	return false;
}

void ConsoleManager::stop()
{
	if (scheduler)
	{
		stopScheduler();
	}
	this->running = false; // Set running to false to stop the console manager
}

void ConsoleManager::readConfigFile()
{
	ifstream configFile("config.txt");
	if (!configFile) {
		std::cerr << "Failed to open config file.\n";
		return;
	}

	unordered_map<string, string> config;
	string line;

	while (getline(configFile, line)) {
		istringstream iss(line);
		string key, value;

		if (!(iss >> key)) continue;  // skip empty lines or invalid lines

		// If value starts with a quote, read until closing quote
		if (iss >> ws && iss.peek() == '"') {
			iss.get();  // consume opening quote
			getline(iss, value, '"'); // read until closing quote
		}
		else {
			iss >> value;  // read next token as value
		}

		config[key] = value;
	}

	configFile.close();

	int numOfCore = config["num-cpu"].empty() ? 1 : stoi(config["num-cpu"]);
	string algorithm = config["scheduler"];
	uint64_t quantumCycles = config["quantum-cycles"].empty() ? 1 : std::stoull(config["quantum-cycles"]);
	uint64_t batchProcessFreq = config["batch-process-freq"].empty() ? 1 : std::stoull(config["batch-process-freq"]);
	uint64_t minIns = config["min-ins"].empty() ? 1 : std::stoull(config["min-ins"]);
	uint64_t maxIns = config["max-ins"].empty() ? 1 : std::stoull(config["max-ins"]);
	uint64_t delayPerExecution = config["delay-per-exec"].empty() ? 0 : std::stoull(config["delay-per-exec"]);
	uint32_t maxOverallMemory = config["max-overall-mem"].empty() ? 1024 : std::stoul(config["max-overall-mem"]);
	uint32_t memoryPerFrame = config["mem-per-frame"].empty() ? 64 : std::stoul(config["mem-per-frame"]);
	uint32_t minMemoryPerProcess = config["min-mem-per-proc"].empty() ? 64 : std::stoul(config["min-mem-per-proc"]);
	uint32_t maxMemoryPerProcess = config["min-mem-per-proc"].empty() ? 256 : std::stoul(config["max-mem-per-proc"]);

	if (algorithm == "fcfs") {
		scheduler = make_shared<Scheduler>(Scheduler::FCFS, numOfCore, quantumCycles, batchProcessFreq, minIns, maxIns, delayPerExecution, maxOverallMemory, memoryPerFrame, minMemoryPerProcess, maxMemoryPerProcess);
	}
	else if (algorithm == "rr") {
		scheduler = make_shared<Scheduler>(Scheduler::RR, numOfCore, quantumCycles, batchProcessFreq, minIns, maxIns, delayPerExecution, maxOverallMemory, memoryPerFrame, minMemoryPerProcess, maxMemoryPerProcess);
	}
	else {
		cerr << "Unknown scheduling algorithm: " << algorithm << endl;
	}
}

void ConsoleManager::initScheduler()
{
	if (!scheduler) {
		readConfigFile();
	}
	else {
		cerr << "Scheduler is already initialized." << endl;
	}
}

void ConsoleManager::runScheduler()
{
	if (scheduler) {
		scheduler->run();
	}
	else {
		cerr << "Scheduler is not initialized." << endl;
	}
}

void ConsoleManager::stopScheduler()
{
	this->scheduler->stop(); // stop the scheduler
}

void ConsoleManager::listProcesses(bool outToFile)
{
	if (scheduler) {
		if (outToFile)
		{
			ofstream outFile("csopesy-log.txt");  // Open the file for writing
			if (!outFile.is_open()) {
				std::cerr << "Error: Could not open process_status.txt for writing.\n";
				return;
			}
			scheduler->printProcessesStatus(outFile);
			outFile.close();
		}
		else
		{
			scheduler->printProcessesStatus(cout);	
		}

	}
	else {
		cerr << "Scheduler is not initialized." << endl;
	}
}

bool ConsoleManager::isSchedulerInitialized()
{
	if (scheduler)
	{
		return true;
	}
	return false;
}

void ConsoleManager::generateProcesses()
{
	this->scheduler->generateProcesses();
}

void ConsoleManager::stopGenerationOfProcesses()
{
	this->scheduler->stopGenerationOfProcesses();
}



