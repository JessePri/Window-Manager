#include "Displays.h"
#include <Windows.h>
#include <string>
#include <fstream>
#include <iostream>


using std::wcout;
using std::endl;
using std::ofstream;
using std::vector;
using std::wstring;
using std::to_wstring;


vector<Displays::Display> Displays::displays;

// Finds all the displays (not tested for more than one display currently)
void Displays::Initialize() {
	MONITORENUMPROC p = FindMonitor;
	LPARAM toIgnore = 0;
	EnumDisplayMonitors(NULL, NULL, p, toIgnore);
}

BOOL CALLBACK Displays::FindMonitor(HMONITOR h, HDC ignore1, LPRECT rect, LPARAM ignore2) {
	MONITORINFO monitorInfo;
	monitorInfo.cbSize = sizeof(monitorInfo);
	GetMonitorInfo(h, &monitorInfo);
#ifdef DEBUG
	wcout << L"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
	wcout << monitorInfo.rcWork.left << endl;
	wcout << monitorInfo.rcWork.right << endl;
	wcout << monitorInfo.rcWork.top << endl;
	wcout << monitorInfo.rcWork.bottom << endl;
	wcout << L"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
#endif
	displays.emplace_back(h, monitorInfo.rcWork);

	return true;
}

wstring Displays::ToString() {
	wstring toReturn = L"";
	for (auto& display : displays) {
		toReturn += L"-------------------------------------------\n";
		toReturn += display.ToString();
		toReturn += L"-------------------------------------------\n";
	}
	return toReturn;
}

wstring Displays::Display::ToString() {
	wstring toReturn = L"Woring Area Dimensions: \n";
	toReturn += L"Left: " + to_wstring(workArea.left) + L"\n";
	toReturn += L"Right: " + to_wstring(workArea.right) + L"\n";
	toReturn += L"Top: " + to_wstring(workArea.top) + L"\n";
	toReturn += L"Bottom: " + to_wstring(workArea.bottom) + L"\n";
	return toReturn;
}
