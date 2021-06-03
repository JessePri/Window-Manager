#pragma once


#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include "Displays.h"
#include "AppManager.h"
#include <unordered_map>


class Start {
public:
	static void StartManager();
	static void Initialize();
	Start() = delete;
private:
	struct HotkeyData {				// Used to record data from a parser and then register a key
		unsigned int modifiers;
		unsigned int virtualKey;
		int id;
	};

	static void RegisterHotKeyFromData(const HotkeyData&);

	static void ParseHotKeyFile(const std::wstring&);

	static void ParseHotKeyFileToData(std::wifstream&);

	static const int SELECT_NUMBER = 0b111001;
};


