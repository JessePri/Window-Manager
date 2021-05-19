#pragma once


#include <iostream>
#include <string>
#include <Windows.h>
#include "WinHWNDPrinter.h"
#include "Displays.h";
#include "ProcessInfo.h"
#include "AppManager.h"
#include <unordered_map>


class Start {
public:
	static void StartManager();
	static void Initialize();
	Start() = delete;
private:
	static const int SELECT_NUMBER = 0b111001;
};


