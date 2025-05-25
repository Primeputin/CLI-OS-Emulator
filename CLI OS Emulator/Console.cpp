#include "Console.h"

void Console::clear()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
void Console::printOutInvalidCommand(string command)
{
	cout << "You entered: " << command << "\n" << "Command invalid" << "\n\n";
}

void Console::show_dateTime(time_t time)
{
    tm local_time = {};
    // Use thread-safe functions to get local time
    #if defined(_WIN32)
        localtime_s(&local_time, &time);  // Windows
    #else
        localtime_r(&now, &time);  // POSIX (Linux/macOS)
    #endif
    cout << put_time(&local_time, "%m/%d/%Y, %I:%M:%S %p") << '\n';
}

void Console::header()
{
	cout << "CSOPESY CLI OS Emulator" << endl;
}

void Console::header(Process process)
{
    cout << "CSOPESY CLI OS Emulator" << endl;
}

vector<string> Console::getSpacedTexts(string command)
{
    vector<string> tokens;
    stringstream parser(command);
    string token;

    while (parser >> token) {
        tokens.push_back(token);
    }

    return tokens;
}