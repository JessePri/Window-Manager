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
#include <wchar.h>
#include <list>
#define DEBUG


using std::list;
using std::queue;
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

unsigned int AppManager::lastProfileIndex = -1;
AppManager::LaunchUpdateMap AppManager::launchUpdateMap;
unordered_set<HWND> AppManager::handlesUsed;
unordered_set<HWND> AppManager::allHandles;
AppManager::ModeMap AppManager::modes;
AppManager::WinMap AppManager::windowedApps;
wstring AppManager::currentMode;
bool AppManager::firstLaunch = true;
bool AppManager::cleared = true;

// Gets the state of the computer
void AppManager::Initialize() {
	GetAllHandles();
	ReadProfilesConstrained(L"ProfileTest.txt");
}

// Finds all valid window applications
void AppManager::GetAllHandles() {
	allHandles.clear();
	LPARAM ignored = 0;
	EnumWindows(UpdateAllHandles, ignored);
}

// Updates the state of the backend after it is first run
void AppManager::UpdateAllWindowedApplications() {
	LPARAM ignored = 0;
	EnumWindows(WindowUpdater, ignored);
	launchUpdateMap.clear();
	firstLaunch = true;
}


// Finds all valid handles that are on the machine
BOOL AppManager::UpdateAllHandles(_In_ HWND hwnd, LPARAM) {
	Application app(hwnd);

	if (!app.IsValid()) {
		return true;
	}

	allHandles.emplace(hwnd);
}

// Updates the windows that are not currently valid, and adds any new applications into the app
BOOL AppManager::WindowUpdater(_In_ HWND hwnd, LPARAM) {
	Application app(hwnd);
	auto iter = windowedApps.find(app.GetWindowModulePath());
	auto launchUpdateIter = launchUpdateMap.find(app.GetWindowModulePath());

	if (!app.IsValid()) {		// Is the app itself a valid app from the users point of view
		return true;
	}

#ifdef DEBUG
	PrintLaunchUpdateMap();
#endif

	if (!app.IsStillValid()) {		// Solves an edge case where a window handle can be recycled
		handlesUsed.erase(hwnd);
	}

	if (!(allHandles.find(hwnd) == allHandles.end())) {		// If the handle is not new in the whole application
		return true;
	}

	if (!(handlesUsed.find(hwnd) == handlesUsed.end())) {	// Checks if the hwnd is still used in the application (might be redundant)
	#ifdef DEBUG
		wcout << "USED HANDLE" << endl;
		app.PrintApplicaiton();
	#endif
		return true;
	}

	if (launchUpdateIter != launchUpdateMap.end()) {
		unsigned int index = *launchUpdateIter->second.begin();

		if (iter == windowedApps.end()) {		// This if statement checks if the app is infact completely new app and a new type of app
		#ifdef DEBUG
			wcout << "launchUpdateIter->second.front(): " << index << endl;
			app.PrintApplicaiton();
		#endif
			handlesUsed.emplace(hwnd);
			unordered_map<unsigned int, Application> temp;
			wstring key = app.GetWindowModulePath();
			temp.emplace(index, std::move(app));
			launchUpdateIter->second.erase(index);

			if (launchUpdateIter->second.empty()) {
				launchUpdateMap.erase(app.GetWindowModulePath());
			}

			windowedApps.emplace(key, std::move(temp));
		} else {
		#ifdef DEBUG
			wcout << "index: " << index << endl;
			app.PrintApplicaiton();
		#endif
			handlesUsed.emplace(hwnd);
			wstring key = app.GetWindowModulePath();
			iter->second.erase(index);
			iter->second.emplace(index, std::move(app));
			launchUpdateIter->second.erase(index);

			if (launchUpdateIter->second.empty()) {
				launchUpdateMap.erase(app.GetWindowModulePath());
			}
		}
	}
	return true;
}

const AppManager::WinMap& AppManager::GetWindowedApps() {
	return windowedApps;
}

void AppManager::ReadProfilesConstrained(const WCHAR* filePath) {
	queue<pair<wstring, unsigned int>> modeAndProfileCounts;
	wifstream input(filePath);
	int count = -1;

	if (input.is_open()) {
		getline(input, currentMode);
		ReadModeData(input, modeAndProfileCounts);

		while (!input.eof()) {
			if (count == -1) {
				vector<Profile> temp;
				modes.emplace(modeAndProfileCounts.front().first, std::move(temp));
				++count;
			}

			Profile toInsert(input, true);
			modes[modeAndProfileCounts.front().first].push_back(std::move(toInsert));
			++count;

			if (count > modeAndProfileCounts.front().second) {
				modeAndProfileCounts.pop();
				break;
			}
		}
	} else {
		wcout << "FILE NOT OPEN" << endl;
	}
}

void AppManager::ReadModeData(wifstream& input, queue<pair<wstring, unsigned int>>& modeAndProfileCounts) {
	wstring temp;
	wstring mode;
	unsigned int count = 0;

	while (getline(input, temp)) {
		if (temp == L"END") {
			break;
		} else if (count == 0) {
			mode = temp;
		} else if (count == 1) {
			modeAndProfileCounts.emplace(mode, stoi(temp));
			count = 0;
		}

		++count;
	}
}


AppManager::Profile::Profile(wifstream& input, bool constrained) {
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
	if (index >= modes[currentMode].size()) {
		wcout << "Invalid Profile" << endl;
		return;
	} 

	for (const AppManager::Profile::MoveInstruction& instruction : modes[currentMode][index].instructions) {
		wcout << "Attempting to RunInstruction" << endl;
		RunInstruction(instruction);
	}

	lastProfileIndex = index;
	cleared = false;
}

void AppManager::RunInstruction(const AppManager::Profile::MoveInstruction& instruction) {
	WinMap::iterator iter = windowedApps.find(instruction.filePath);
	unordered_map<unsigned int, Application>::iterator appIter;

	if (CheckValidInstruction(instruction, appIter)) {
	#ifdef DEBUG
		wcout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
		wcout << instruction.ToString() << endl;
		appIter->second.PrintApplicaiton();
		wcout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
	#endif
		appIter->second.SetPosition					
		(instruction.x, instruction.y, instruction.cx, instruction.cy, SWP_ASYNCWINDOWPOS);
	}
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

	for (Profile profile : modes[currentMode]) {
		wcout << "#############################################" << endl;
		wcout << profile.ToString() << endl;
		wcout << "#############################################" << endl;
		++count;
	}
}

wstring ConvertToLaunchPath(const wstring& filePath) {
	list<WCHAR> tempStr(filePath.begin(), filePath.end());
	auto end = tempStr.end();
	unsigned char skipCount = 0;

	for (auto iter = tempStr.begin(); iter != end; ++iter) {
		if (*iter == L'\\') {
			iter = tempStr.insert(iter, L'\\');
			++iter;
		}
	}

	auto tempIter = tempStr.begin();
	tempStr.insert(tempIter, L'\"');
	tempIter = tempStr.end();
	tempStr.insert(tempIter, L'\"');
	wstring toReturn(tempStr.begin(), tempStr.end());
	return toReturn;
}

AppManager::Profile::MoveInstruction::MoveInstruction(const PreInstruction& preInstruction) {
	filePath = preInstruction.filePath;
	filePathToLaunch = ConvertToLaunchPath(filePath);
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

void AppManager::ClearProfiles() {
	for (const Profile& p : modes[currentMode]) {
		ClearProfile(p);
	}
}

void AppManager::ClearProfile(const Profile& profile) {
	unordered_map<unsigned int, Application>::iterator appIter;

	for (const Profile::MoveInstruction& instruction : profile.instructions) {
		if (CheckValidInstruction(instruction, appIter)) {
			appIter->second.MinimizeApplication();
		}
	}

	cleared = true;
}

bool AppManager::CheckValidInstruction(const Profile::MoveInstruction& instruction, unordered_map<unsigned int, Application>::iterator& toReturn) {
	WinMap::iterator iter = windowedApps.find(instruction.filePath);

	if (iter != windowedApps.end()) {	// Checks if the an app of that type exists 
		toReturn = iter->second.find(instruction.appIndex);
		if (toReturn == iter->second.end() || !toReturn->second.IsStillValid()) {	//	Checks if this the app is invalid either in index or it was closed
			handlesUsed.erase(toReturn->second.GetHWND());
			wcout << "Invalid instruction!" << endl;
			return false;
		}
	} else {
		wcout << "Invalid instruction!" << endl;
		return false;
	}

	return true;
}

void AppManager::LaunchProfile(unsigned int index) {
	if (firstLaunch) {
		GetAllHandles();
		firstLaunch = false;
	}

	if (index >= modes[currentMode].size()) {
		wcout << "Invalid Profile To Launch" << endl;
		return;
	}

	for (const Profile::MoveInstruction& instruction : modes[currentMode][index].instructions) {
		LaunchWindowFromMoveInstruction(instruction);
	}
}

void AppManager::LaunchWindowFromMoveInstruction(const Profile::MoveInstruction& instruction) {
	std::unordered_map<unsigned int, Application>::iterator iter;		// This variable does not have use in this context

	auto launchUpdateIter = launchUpdateMap.find(instruction.filePath);

	if (CheckValidInstruction(instruction, iter)) {		// If the instruction is valid then you don't need to do anything
		return;
	} else if (launchUpdateIter != launchUpdateMap.end()
		&& launchUpdateIter->second.find(instruction.appIndex) != launchUpdateIter->second.end()) {		// If the the window was launched already do nothing
		return;
	}

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	WCHAR* path = _wcsdup(instruction.filePathToLaunch.c_str());

	if (CreateProcess(0, path, 0, 0, 0, 0, 0, 0, &si, &pi)) {					// If the process is launched then add a app index into the update map 
		wcout << "LAUNCHED" << endl;											// for it to be handles by the WindowUpdate function 
		if (launchUpdateIter == launchUpdateMap.end()) {
			unordered_set<unsigned int> temp;
			temp.emplace(instruction.appIndex);
			launchUpdateMap.emplace(instruction.filePath, std::move(temp));
		} else {
			launchUpdateIter->second.emplace(instruction.appIndex);
		}
	}
#ifdef DEBUG
	PrintLaunchUpdateMap();
#endif 
}

void AppManager::PrintLaunchUpdateMap() {
	for (auto& p : launchUpdateMap) {
		wcout << "---------------------------" << endl;
		wcout << p.first << endl;
		for (auto& q : p.second) {
			wcout << q << ",";
		}
		wcout << endl;
		wcout << "---------------------------" << endl;
	}
}

void AppManager::AltTabProfile() {
	if (cleared) {
		RunProfile(lastProfileIndex);
	} else if (lastProfileIndex != -1) {						// lastProfile index is invalid if it is -1 only. It can only be set by RunProfile
		ClearProfile(modes[currentMode][lastProfileIndex]);		// which checks if it is valid already
	}
}

