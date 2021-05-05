// PorjectTestCLI.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <Windows.h>
#include "WinHWNDPrinter.h"
#include "ProcessInfo.h"
#include "AppManager.h"
#include <iostream>


using std::wcout;
using std::endl;


int main()
{
	//WinHWNDPrinter::PrintHWNDInfo();
	wcout << L"################################################################" << endl;
	wcout << L"################################################################" << endl;
	wcout << L"################################################################" << endl;
	wcout << L"################################################################" << endl;
	wcout << L"################################################################" << endl;
	AppManager::GetAllWindowedApplications();
	AppManager::PrintWindowedApps();
	AppManager::PrintSizeOfWindowedApps();
}