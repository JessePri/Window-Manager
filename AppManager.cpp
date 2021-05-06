#include "AppManager.h"
#include "Application.h"
#include "Displays.h"
#include <Windows.h>
#include <Psapi.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>


using std::wstring;
using std::wifstream;
using std::vector;
using std::getline;
using std::stoi;
using std::wcout;
using std::endl;
using std::to_wstring;


vector<AppManager::Profile> AppManager::profiles;
Application::WinMap AppManager::windowedApps;

void AppManager::Initialize() {
	Displays::Initialize();
	GetAllWindowedApplications();
}

void AppManager::GetAllWindowedApplications() {
	windowedApps.clear();
	LPARAM ignored = 0;
	EnumWindows(WindowConstructor, ignored);
}

BOOL AppManager::WindowConstructor(_In_ HWND hwnd, LPARAM IGNORED) {
	Application app(hwnd);
	if (!app.IsValid()) {
		wcout << "INVALID" << endl;
		return true;
	}
	auto iter = windowedApps.find(app.GetWindowModulePath());
	if (iter == windowedApps.end()) {
		wcout << "Module Path Not Found" << endl;
		vector<Application> temp;
		temp.push_back(std::move(app));
		wstring key = temp[0].GetWindowModulePath();
		windowedApps.emplace(key,std::move(temp));
	} else {
		iter->second.emplace_back(std::move(app));
	}
	return true;
}

const Application::WinMap& AppManager::GetWindowedApps() {
	return windowedApps;
}

void AppManager::ReadProfilesConstrained(const char* filePath) {
	wifstream input(filePath);
	if (input.is_open()) {
		while (!input.eof()) {
			profiles.emplace_back(input, true);
		}
	} else {
		// Do some logging
	}
}


AppManager::Profile::Profile(wifstream& input, bool constrained) {
	// The boolean will be used when the Advanced Profiles are implemented
	// For now it should be ignored
	ReadProfileConstrained(input);
}

/* You wrote this wrong...
*	- You are not reading the complete data set into this
*		1. Per App window move you are reading 8 THINGS (not 6)
*		2. You are computing the move instruction not making it
*		4. Before this you need this application to find 
*			the size of the screen
*		5. After you do the above you need to write the ToString
*			and Print methods
*/
void AppManager::Profile::ReadProfileConstrained(wifstream& input) {
	wstring temp;
	unsigned char count = 0;
	while (getline(input, temp)) {
		if (temp._Equal(L"END")) {
			break;
		}
		instructions.emplace_back();
		if (count == 0) {
			instructions.back().filePath = temp;
			++count;
		} else if (count == 1) {
			instructions.back().appIndex = stoi(temp);
			++count;
		} else if (count == 2) {
			instructions.back().x = stoi(temp);
			++count;
		} else if (count == 3) {
			instructions.back().y = stoi(temp);
			++count;
		} else if (count == 4) {
			instructions.back().cx = stoi(temp);
			++count;
		} else if (count == 5) {
			instructions.back().cy = stoi(temp);
			count = 0;
		}
	}
	if (count > 0) {
		instructions.pop_back();
	}
}

void AppManager::RunProfile(unsigned int index) {
	for (AppManager::Profile::MoveInstruction instruction : profiles[index].instructions) {
		RunInstruction(instruction);
	}
}

void AppManager::RunInstruction(const AppManager::Profile::MoveInstruction& instruction) {
	auto iter = windowedApps.find(instruction.filePath);
	if (iter == windowedApps.end()) {
		return;
	}
	iter->second[instruction.appIndex].SetPosition
	(instruction.x, instruction.y, instruction.cx, instruction.cy, SWP_ASYNCWINDOWPOS | SWP_SHOWWINDOW);
}


void AppManager::PrintWindowedApps() {
	for (auto& p : windowedApps) {
		for (auto& a : p.second) {
			wcout << L"/////////////////////////////////////////////" << endl;
			wcout << "HWND: " << a.GetHWND() << endl;
			wcout << a.ToString() << endl;
			wcout << L"/////////////////////////////////////////////" << endl;
		}
	}
}

void AppManager::PrintSizeOfWindowedApps() {
	wcout << windowedApps.size() << endl;
}

wstring AppManager::Profile::MoveInstruction::ToString() {
	wstring toReturn = filePath;
	toReturn += L"App Index: " + to_wstring(appIndex);
	toReturn += L"x: " + to_wstring(x) + L"\n";
	toReturn += L"y: " + to_wstring(x) + L"\n";
	toReturn += L"cx: " + to_wstring(cx) + L"\n";
	toReturn += L"cy: " + to_wstring(cy) + L"\n";
	return toReturn;
}

wstring AppManager::Profile::ToString() {
	wstring toReturn = L"";
	for (MoveInstruction instruction : instructions) {
		toReturn += L"-----------------------------------------\n";
		toReturn += instruction.ToString();
		toReturn += L"-----------------------------------------\n";
	}
	return toReturn;
}

void AppManager::PrintProfiles() {
	for (Profile profile : profiles) {
		wcout << "#############################################" << endl;
		wcout << profile.ToString() << endl;
		wcout << "#############################################" << endl;
	}
}