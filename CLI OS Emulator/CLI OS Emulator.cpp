#include <iostream>
#include <string>
#include "MainConsole.h"
#include "ProcessConsole.h"
#include "ConsoleManager.h"

using namespace std;


int main() {
    ConsoleManager::getInstance();
    ConsoleManager::getInstance().run();

    return 0;
}
