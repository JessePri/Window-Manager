#include "AppManager.h"
#include "Application.h"
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


vector<AppManager::Profile> AppManager::profiles;
Application::WinMap AppManager::windowedApps;

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
			wcout << a.ToString() << endl;
			wcout << L"/////////////////////////////////////////////" << endl;
		}
	}
}

void AppManager::PrintSizeOfWindowedApps() {
	wcout << windowedApps.size() << endl;
}