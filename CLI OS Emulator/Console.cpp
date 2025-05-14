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

void Console::show_dateTime()
{
    time_t now = time(nullptr);
    tm local_time = {};
    // Use thread-safe functions to get local time
    #if defined(_WIN32)
        localtime_s(&local_time, &now);  // Windows
    #else
        localtime_r(&now, &local_time);  // POSIX (Linux/macOS)
    #endif
    cout << put_time(&local_time, "%m/%d/%Y, %I:%M:%S %p") << '\n';
}
bool Console::getIsOnDisplay() const
{
    return isOnDisplay;
}
void Console::setIsOnDisplay(bool isOnDisplay)
{
	this->isOnDisplay = isOnDisplay;
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