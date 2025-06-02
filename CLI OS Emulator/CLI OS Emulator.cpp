#include <iostream>
#include <string>
#include "MainConsole.h"
#include "ProcessConsole.h"
#include "ConsoleManager.h"

using namespace std;


int main() {

	ConsoleManager::getInstance()->initialize();
	while (ConsoleManager::getInstance()->running)
	{
		ConsoleManager::getInstance()->currentConsole->getCommand();
	}
	ConsoleManager::getInstance()->destroy();
    return 0;
}
