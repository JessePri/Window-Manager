#include "Start.h"

#include <iostream>
#include <string>
#include <Windows.h>
#include "WinHWNDPrinter.h"
#include "Displays.h";
#include "ProcessInfo.h"
#include "AppManager.h"


using std::string;
using std::cin;
using std::wcout;
using std::endl;


void Start::Initialize() {
	Displays::Initialize();
	AppManager::Initialize();
	RegisterHotKey(NULL, 1, MOD_NOREPEAT | MOD_CONTROL | MOD_ALT, 0x31);
	RegisterHotKey(NULL, 2, MOD_NOREPEAT | MOD_CONTROL | MOD_ALT, 0x32);
	RegisterHotKey(NULL, 3, MOD_NOREPEAT | MOD_CONTROL | MOD_ALT, 0x33);
	RegisterHotKey(NULL, 0, MOD_NOREPEAT | MOD_CONTROL | MOD_ALT, 0x30);
}

void Start::StartManager() {
	BOOL errorState;
	MSG msg;
	while ((errorState = GetMessage(&msg, NULL, 0, 0)) != 0) {
		if (errorState == -1) {
			return;
		} else if (msg.message == WM_HOTKEY) {
			wcout << "HOT KEY PRESSED" << endl;
			wcout << msg.wParam << endl;
			wcout << "---------------------------------------------" << endl;
			if (msg.wParam != 0) {
				AppManager::RunProfile(msg.wParam - 1);
			} else if (msg.wParam == 0) {
				AppManager::UpdateAllWindowedApplications();
			}
		}
	}
	wcout << "exit" << endl;
}