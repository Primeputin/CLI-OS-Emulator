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
	if (consoleTable.find(name) != this->consoleTable.end()) {
		std::cerr << "Screen name " << name << " already exists. Please use a different name." << std::endl;
		return;
	}

	// <-- Temporary -->
	vector<shared_ptr<ICommand>> commandList;
	for (int j = 0; j < 100; j++)
	{
		commandList.push_back(make_shared<PrintCommand>(100, "Hello World!"));
	}

	shared_ptr<Process> process = make_shared<Process>(100, name, commandList);

	// <-- Temporary -->

	shared_ptr<ProcessConsole> newConsole = make_shared<ProcessConsole>(process);

	consoleTable.insert({ newConsole->getName(), newConsole}); // temporary

	cout << "Screen name: " << name << " created successfully." << std::endl;

	switchToProcessConsole(newConsole->getName());
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
	int quantumCycles = config["quantum-cycles"].empty() ? 1 : stoi(config["quantum-cycles"]);
	int batchProcessFreq = config["batch-process-freq"].empty() ? 1 : stoi(config["batch-process-freq"]);
	int minIns = config["min-ins"].empty() ? 1 : stoi(config["min-ins"]);
	int maxIns = config["max-ins"].empty() ? 1 : stoi(config["max-ins"]);
	int delayPerExecution = config["delay-per-exec"].empty() ? 0 : stoi(config["delay-per-exec"]);

	if (algorithm == "fcfs") {
		scheduler = make_shared<Scheduler>(Scheduler::FCFS, numOfCore, quantumCycles, batchProcessFreq, minIns, maxIns, delayPerExecution);
	}
	else if (algorithm == "r") {
		scheduler = make_shared<Scheduler>(Scheduler::RR, numOfCore, quantumCycles, batchProcessFreq, minIns, maxIns, delayPerExecution);
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

void ConsoleManager::listProcesses()
{
	if (scheduler) {
		scheduler->printProcessesStatus();
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



