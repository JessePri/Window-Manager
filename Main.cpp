#include <iostream>
#include <string>
#include <Windows.h>
#include "Displays.h"
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