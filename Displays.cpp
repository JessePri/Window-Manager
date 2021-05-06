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


vector<Displays::Display> displays;

void Displays::Initialize() {
	MONITORENUMPROC p = FindMonitor;
	LPARAM toIgnore;
	EnumDisplayMonitors(NULL, NULL, p, toIgnore);
}

// May of may not work properly
// If it doesn't work properly with multip monitors we would need to use EnumDisplayDevices

BOOL CALLBACK Displays::FindMonitor(HMONITOR h, HDC ignore1, LPRECT rect, LPARAM ignore2) {
	displays.emplace_back(h, *rect);
}

wstring Displays::ToString() {
	wstring toReturn = L"";
	for (auto display : displays) {
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
