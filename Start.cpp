#include "Start.h"

#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include "WinHWNDPrinter.h"
#include "Displays.h"
#include "ProcessInfo.h"
#include "AppManager.h"


using std::wstring;
using std::string;
using std::cin;
using std::wcout;
using std::endl;
using std::wifstream;


void Start::Initialize() {
	Displays::Initialize();
	AppManager::Initialize();
	ParseHotKeyFile(L"HotkeyTest.txt");
	//RegisterHotKey(NULL, 1, MOD_NOREPEAT | MOD_CONTROL | MOD_ALT, 0x31);
	//RegisterHotKey(NULL, 2, MOD_NOREPEAT | MOD_CONTROL | MOD_ALT, 0x32);
	//RegisterHotKey(NULL, 3, MOD_NOREPEAT | MOD_CONTROL | MOD_ALT, 0x33);
	//RegisterHotKey(NULL, 0, MOD_NOREPEAT | MOD_CONTROL | MOD_ALT, 0x30);
	wcout << (int)(MOD_NOREPEAT | MOD_CONTROL | MOD_ALT) << endl;
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
			if (msg.wParam > 100) {
				unsigned int profileIndex = msg.wParam - 100;
				// Launch Profile according to the profile index
				// Run the profile
			} else if (msg.wParam > 0) {
				AppManager::RunProfile(msg.wParam - 1);
			} else if (msg.wParam == 0) {
				AppManager::UpdateAllWindowedApplications();
			}
		}
	}
	wcout << "exit" << endl;
}

void Start::ParseHotKeyFile(const wstring& filePath) {
	wifstream fileStream(filePath);
	if (fileStream.is_open()) {
		while (!fileStream.eof()) {
			ParseHotKeyFileToData(fileStream);
		}
	} else {
		wcout << "Hotkey file cannot be opened..." << endl;
	}
}

void Start::ParseHotKeyFileToData(wifstream& fileStream) {
	unsigned int count = 0;
	wstring temp;
	HotkeyData hotkeyData;
	while (getline(fileStream, temp)) {
		if (count == 0) {
			hotkeyData.id = stoi(temp);
		} else if (count == 1) {
			hotkeyData.modifiers = stoi(temp);
		} else if (count == 2) {
			hotkeyData.virtualKey = stoi(temp);
			break;
		}
		++count;
	}
	wcout << "------------------------------------------" << endl;
	wcout << "id: " << hotkeyData.id << endl;
	wcout << "modifiers: " << hotkeyData.modifiers << endl;
	wcout << "virtual key: " << hotkeyData.virtualKey << endl;
	wcout << "------------------------------------------" << endl;
	RegisterHotKeyFromData(hotkeyData);
}

void Start::RegisterHotKeyFromData(const HotkeyData& hotkeyData) {
	RegisterHotKey(NULL, hotkeyData.id, hotkeyData.modifiers, hotkeyData.virtualKey);
}