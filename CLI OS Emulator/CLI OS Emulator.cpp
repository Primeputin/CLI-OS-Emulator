#include <iostream>
#include <string>
using namespace std;

// ANSI escape codes for color
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define RESET   "\033[0m"

void printCSOPESYBanner() {
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

void recognizedCommand(string command)
{
    cout << command << " recognized, Doing something." << endl;
}

void initialize(string command)
{
    recognizedCommand(command);
}

void screen(string command)
{
    recognizedCommand(command);
}

void schedulerTest(string command)
{
    recognizedCommand(command);
}

void schedulerStop(string command)
{
    recognizedCommand(command);
}

void reportUtil(string command)
{
    recognizedCommand(command);
}

int main() {
    printCSOPESYBanner();
    string command;
    while (true)
    {
        cout << "Enter a comamnd: ";
        getline(cin, command);
        if (command == "initialize")
        {
            initialize(command);
        }
        else if (command == "screen")
        {
            screen(command);
        }
        else if (command == "scheduler-test")
        {
            schedulerTest(command);
        }
        else if (command == "scheduler-stop")
        {
            schedulerStop(command);
        }
        else if (command == "report-util")
        {
            reportUtil(command);
        }
        else if (command == "exit")
        {
            recognizedCommand(command);
        }
        else if (command == "clear") {
            recognizedCommand(command);
#ifdef _WIN32
            system("cls");
#else
            system("clear");
#endif
            printCSOPESYBanner();
        }
        else {
            cout << "You entered: " << command << "\n" << "Command invalid" << "\n\n";
        }
    }
    return 0;
}
