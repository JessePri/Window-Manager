// PorjectTestCLI.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <Windows.h>
#include "WinHWNDPrinter.h"
#include "Displays.h";
#include "ProcessInfo.h"
#include "AppManager.h"
#include "Start.h"


using std::string;
using std::cin;
using std::wcout;
using std::endl;


int main()
{
	Start::Initialize();
	Start::StartManager();
	return 0;
	//WinHWNDPrinter::PrintHWNDInfo();
	wcout << L"################################################################" << endl;
	wcout << L"################################################################" << endl;
	wcout << L"################################################################" << endl;
	wcout << L"################################################################" << endl;
	wcout << L"################################################################" << endl;
	Displays::Initialize();
	AppManager::Initialize();
	wcout << Displays::ToString() << endl;
	AppManager::PrintWindowedApps();
	AppManager::PrintProfiles();
	cin.get();
	AppManager::RunProfile(0);
	cin.get();
	AppManager::UpdateAllWindowedApplications();
	wcout << L"################################################################" << endl;
	wcout << L"################################################################" << endl;
	wcout << L"################################################################" << endl;
	wcout << L"################################################################" << endl;
	wcout << L"################################################################" << endl;
	cin.get();
	AppManager::RunProfile(0);

	//wcout << L"################################################################" << endl;
	//wcout << L"################################################################" << endl;
	//wcout << L"################################################################" << endl;
	//wcout << L"################################################################" << endl;
	//wcout << L"################################################################" << endl;
	//AppManager::PrintWindowedApps();
}