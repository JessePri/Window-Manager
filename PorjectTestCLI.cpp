#include <iostream>
#include <string>
#include <Windows.h>
#include "WinHWNDPrinter.h"
#include "Displays.h"
#include "ProcessInfo.h"
#include "AppManager.h"
#include "Start.h"


using std::wstring;
using std::cin;
using std::wcout;
using std::endl;


int main() {
	Start::Initialize();
	Start::StartManager();
	return 0;
}