#include "AppManager.h"
#include "Application.h"
#include "Displays.h"
#include <Windows.h>
#include <Psapi.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <exception>
#include <unordered_map>
#include <queue>


using std::wstring;
using std::wifstream;
using std::vector;
using std::getline;
using std::stoi;
using std::wcout;
using std::endl;
using std::to_wstring;
using std::cout;
using std::exception;
using std::unordered_map;
using std::pair;
using std::unordered_set;
using std::priority_queue;


AppManager::UpdateMap AppManager::updateMap;
unordered_set<HWND> AppManager::handleSet;
vector<AppManager::Profile> AppManager::profiles;
AppManager::WinMap AppManager::windowedApps;
unordered_map<wstring, unsigned int> AppManager::constructionIndexes;

// Gets the state of the computer
void AppManager::Initialize() {
	GetAllWindowedApplications();
	ReadProfilesConstrained(L"ProfileTest.txt");
}

// Finds all valid window applications
void AppManager::GetAllWindowedApplications() {
	windowedApps.clear();
	LPARAM ignored = 0;
	EnumWindows(WindowConstructor, ignored);
}

// Updates the state of the backend after it is first run
void AppManager::UpdateAllWindowedApplications() {
	LPARAM ignored = 0;
	updateMap.clear();
	MarkWindowUpdates();
	EnumWindows(WindowUpdater, ignored);
}

// Finds all windows that are invalid and marks them for the updater
void AppManager::MarkWindowUpdates() {
	// This code iterates over all the applications and add them to the update map if they are invalid
	for (auto& p1 : windowedApps) {
		for (auto& p2 : p1.second) {
			if (!p2.second.IsStillValid()) {
				wcout << "HWND TO REMOVE: " << p2.second.GetHWND() << endl;
				handleSet.erase(p2.second.GetHWND());
				UpdateMap::iterator updateIter = updateMap.find(p2.second.GetWindowModulePath());
				if (updateIter == updateMap.end()) {
					priority_queue<unsigned int> temp;
					temp.push(p2.first);
					updateMap.emplace(p2.second.GetWindowModulePath(), std::move(temp));
				} else {
					updateIter->second.push(p2.first);
				}
			}											
		}
	}
}

// Constructs each Application class from a window handle
BOOL AppManager::WindowConstructor(_In_ HWND hwnd, LPARAM) {
	Application app(hwnd);
	if (!app.IsValid()) {
		return true;
	}
	handleSet.emplace(hwnd);
	auto iter = windowedApps.find(app.GetWindowModulePath());
	if (iter == windowedApps.end()) {		// If the type of application is new then we will add it along with new catagory in windowedApps
		constructionIndexes.emplace(app.GetWindowModulePath(), 0);
		unordered_map<unsigned int, Application> temp;
		unordered_map<HWND, unsigned int> handleTemp;
		wstring key = app.GetWindowModulePath();
		pair<unsigned int, wstring> hMapPair(0, key);
		temp.emplace(0, std::move(app));
		windowedApps.emplace(key, std::move(temp));
	} else {		// If the application is not of a new type then it is added to a section of that type
		wstring key = app.GetWindowModulePath();
		pair<unsigned int, wstring> hMapPair(constructionIndexes[key] + 1, key);
		windowedApps[key].emplace(++constructionIndexes[key], std::move(app));
	}
	return true;
}

// Updates the windows that are not currently valid, and adds any new applications into the app
BOOL AppManager::WindowUpdater(_In_ HWND hwnd, LPARAM) {
	Application app(hwnd);
	auto iter = windowedApps.find(app.GetWindowModulePath());
	auto updateIter = updateMap.find(app.GetWindowModulePath());
	if (!app.IsValid()) {		// Is the app it self valid (can we access it and is it an actual window?)
		return true;
	} else if (!(handleSet.find(hwnd) == handleSet.end())) {
		app.PrintApplicaiton();
		return true;
	} else if (updateIter != updateMap.end()) {		// Was it previously identified as invalid? Then replace it with this new one.
		auto tempIter = iter->second.find(updateIter->second.top());
		if (tempIter == iter->second.end()) {
			return true;
		}
		tempIter->second = std::move(app);		// This needs to be changed
		handleSet.emplace(hwnd);
		updateIter->second.pop();
	} else if (iter == windowedApps.end()) {		// This if statement checks if the app is infact completely new app and a new type of app
		handleSet.emplace(hwnd);
		constructionIndexes.emplace(app.GetWindowModulePath(), 0);
		unordered_map<unsigned int, Application> temp;
		wstring key = app.GetWindowModulePath();
		temp.emplace(0, std::move(app));
		windowedApps.emplace(key, std::move(temp));
	} else {
		handleSet.emplace(hwnd);
		wstring key = app.GetWindowModulePath();
		iter->second.emplace(++constructionIndexes[key], std::move(app));	// NOTE: not updating this may cause issues 
	}
	return true;
}

const AppManager::WinMap& AppManager::GetWindowedApps() {
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

// Reads the profile-->PreInstruction-->MoveInstruction
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
	WinMap::iterator iter = windowedApps.find(instruction.filePath);
	unordered_map<unsigned int, Application>::iterator appIter;
	if (iter != windowedApps.end()) {	// Checks if the an app of that type exists 
		appIter = iter->second.find(instruction.appIndex);
		if (appIter == iter->second.end() || !appIter->second.IsStillValid()) {	//	Checks if this the app is invalid either in index or it was closed
			handleSet.erase(appIter->second.GetHWND());							//	For the future we may need to add SEH here
			iter->second.erase(instruction.appIndex);
			wcout << "Invalid instruction!" << endl;
			return;
		}
	} else {
		wcout << "Invalid instruction!" << endl;
		return;
	}
	wcout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
	wcout << instruction.ToString() << endl;
	appIter->second.PrintApplicaiton();
	wcout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
	appIter->second.SetPosition
	(instruction.x, instruction.y, instruction.cx, instruction.cy, SWP_ASYNCWINDOWPOS);
}

void AppManager::PrintWindowedApps() {
	for (auto& p : windowedApps) {
		for (auto& a : p.second) {
			wcout << L"/////////////////////////////////////////////" << endl;
			cout << "HWND: " << a.second.GetHWND() << endl;
			wcout << a.second.ToString() << endl;
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

	x = preInstruction.startX * blockX + left - 8;	// These constants are neccessary for things to look good on windows 10
	y = preInstruction.startY * blockY + top;
	cx = preInstruction.widthX * blockX + 16;
	cy = preInstruction.widthY * blockY + 8;

	// Idea is to ensure a full screen even if there is slight (1-2 pixel overlap between windows) 
	if (blockX * preInstruction.totalX < (right - left)) {
		++cx;
	}
	if (blockY * preInstruction.totalY < (bottom - top)) {
		++cy;
	}
}

// Bad code....

//void AppManager::CreateNewWindow(const AppManager::Profile::MoveInstruction& instruction) {
//	CreateProcess(instruction.filePath.c_str(), NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
//	Sleep(1000);
//	EnumWindows(FindNewValidWindow, 0);
//}
//
//// This needs to be modified
//BOOL AppManager::FindNewValidWindow(_In_ HWND hwnd, LPARAM) {
//	Application temp(hwnd);
//	if (temp.GetWindowModulePath() != modulePathToCompare) {
//		return true;
//	}
//	auto iter = windowedApps.find(modulePathToCompare);
//	if (iter == windowedApps.end()) {
//		newValidWindow = std::move(temp);
//	} else {
//		for (Application& app : iter->second) {
//			if (app.GetHWND() == temp.GetHWND()) {
//				return true;
//			}
//		}
//		newValidWindow = std::move(temp);
//	}
//	return false;
//	// Check if the module has an application with the following module path
//	// If it does then check all the hwnd values and return false once you have found that it is not contained
//	// If it doesn't then you would return false
//	// In both cases before returing the newValidWindow should be set
//	// After this continue the logic that you were developing in the RunInstruction(...) function
//}



