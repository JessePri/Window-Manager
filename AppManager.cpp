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
using std::cout;


vector<AppManager::Profile> AppManager::profiles;
Application::WinMap AppManager::windowedApps;

void AppManager::Initialize() {
	GetAllWindowedApplications();
	ReadProfilesConstrained(L"ProfileTest.txt");
}

void AppManager::GetAllWindowedApplications() {
	windowedApps.clear();
	LPARAM ignored = 0;
	EnumWindows(WindowConstructor, ignored);
}

BOOL AppManager::WindowConstructor(_In_ HWND hwnd, LPARAM IGNORED) {
	Application app(hwnd);
	if (!app.IsValid()) {
		return true;
	}
	auto iter = windowedApps.find(app.GetWindowModulePath());
	if (iter == windowedApps.end()) {
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

void AppManager::ReadProfilesConstrained(const WCHAR* filePath) {
	wifstream input(filePath);
	if (input.is_open()) {
		while (!input.eof()) {
			profiles.emplace_back(input, true);
		}
	} else {
		wcout << "FILE NOT OPEN" << endl;
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
	int count = 0;
	PreInstruction preInstruction;
	bool lastIter = false;
	unsigned int rcount = 0;
	while (getline(input, temp)) {
		++rcount;
		if (temp._Equal(L"END")) {
			break;
		}
		if (count == 0) {
			preInstruction.filePath = temp;
			++count;
		} else if (count == 1) {
			preInstruction.appIndex = stoi(temp);
			++count;
		} else if (count == 2) {
			preInstruction.displayID = stoi(temp);
			++count;
		} else if (count == 3) {
			preInstruction.totalX = stoi(temp);
			++count;
		} else if (count == 4) {
			preInstruction.totalY = stoi(temp);
			++count;
		} else if (count == 5) {
			preInstruction.startX = stoi(temp);
			++count;
		} else if (count == 6) {
			preInstruction.startY = stoi(temp);
			++count;
		} else if (count == 7) {
			preInstruction.widthX = stoi(temp);
			++count;
		} else if (count == 8) {
			preInstruction.widthY = stoi(temp);
			lastIter = true;
			count = 0;
		}
		if (lastIter) {
			instructions.emplace_back(preInstruction);
			lastIter = false;
		}
	}
	if (count > 0) {
		instructions.pop_back();
	}
}

void AppManager::RunProfile(unsigned int index) {
	for (AppManager::Profile::MoveInstruction instruction : profiles[index].instructions) {
		wcout << "Attempting to RunInstruction" << endl;
		RunInstruction(instruction);
	}
}

void AppManager::RunInstruction(const AppManager::Profile::MoveInstruction& instruction) {
	auto iter = windowedApps.find(instruction.filePath);
	if (iter == windowedApps.end()) {
		wcout << "Appliction Not Found..." << endl;
		return;
	}
	wcout << instruction.ToString() << endl;
	iter->second[instruction.appIndex].SetPosition
	(instruction.x, instruction.y, instruction.cx, instruction.cy, SWP_ASYNCWINDOWPOS);
	
}


void AppManager::PrintWindowedApps() {
	for (auto& p : windowedApps) {
		for (auto& a : p.second) {
			wcout << L"/////////////////////////////////////////////" << endl;
			cout << "HWND: " << a.GetHWND() << endl;
			wcout << a.ToString() << endl;
			wcout << L"/////////////////////////////////////////////" << endl;
		}
	}
}

void AppManager::PrintSizeOfWindowedApps() {
	wcout << windowedApps.size() << endl;
}

wstring AppManager::Profile::MoveInstruction::ToString() const {
	wstring toReturn = filePath;
	toReturn += L"\nApp Index: " + to_wstring(appIndex) + L"\n";
	toReturn += L"x: " + to_wstring(x) + L"\n";
	toReturn += L"y: " + to_wstring(y) + L"\n";
	toReturn += L"cx: " + to_wstring(cx) + L"\n";
	toReturn += L"cy: " + to_wstring(cy) + L"\n";
	return toReturn;
}

wstring AppManager::Profile::ToString() {
	unsigned int count = 0;
	wstring toReturn = L"";
	for (MoveInstruction instruction : instructions) {
		toReturn += L"-----------------------------------------\n";
		toReturn += instruction.ToString();
		toReturn += L"-----------------------------------------\n";
		++count;
	}
	return toReturn;
}

void AppManager::PrintProfiles() {
	unsigned int count = 0;
	for (Profile profile : profiles) {
		wcout << "#############################################" << endl;
		wcout << profile.ToString() << endl;
		wcout << "#############################################" << endl;
		++count;
	}
}

AppManager::Profile::MoveInstruction::MoveInstruction(const PreInstruction& preInstruction) {
	filePath = preInstruction.filePath;
	appIndex = preInstruction.appIndex;

	double left = (double)Displays::displays[preInstruction.displayID].workArea.left;
	double right = (double)Displays::displays[preInstruction.displayID].workArea.right;
	double top = (double)Displays::displays[preInstruction.displayID].workArea.top;
	double bottom = (double)Displays::displays[preInstruction.displayID].workArea.bottom;

	double blockX = (right - left) / preInstruction.totalX;
	double blockY = (bottom - top) / preInstruction.totalY;

	x = preInstruction.startX * blockX + left - 8;
	y = preInstruction.startY * blockY + top;
	cx = preInstruction.widthX * blockX + 16;
	cy = preInstruction.widthY * blockY;

	// Idea is to ensure a full screen even if there is slight (1-2 pixel overlap between windows)
	if (blockX * preInstruction.totalX < (right - left)) {
		++cx;
	}
	if (blockY * preInstruction.totalY < (bottom - top)) {
		++cy;
	}
}

