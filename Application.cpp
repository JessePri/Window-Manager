#include "Application.h"
#include <exception>
#include <Windows.h>
#include <Psapi.h>
#include <string>
#include <iostream>
#define UNICODE

using std::cout;
using std::wcout;
using std::endl;
using std::wstring;
using std::exception;
using std::to_wstring;


/* Things you will eventually need to fix (or not)
*	- This doesn't really catch system exceptions you could
*		1. Make part the code that reads and gets the proccess handle part of a
*			__try __except with a helper fucntion to do the constructing 
*		2. You could ignore this issue since EnumWindows is really fast so a user would have to run 
*				the app then close a window imediatley for it to crash
*/
Application::Application(_In_ HWND winHandle) {
	hwnd = winHandle;
	try {		
		WINDOWINFO winfo;
		GetWindowInfo(hwnd, &winfo);
		x = winfo.rcWindow.left;
		y = winfo.rcWindow.top;
		width = (winfo.rcWindow.right - x);
		depth = (winfo.rcWindow.bottom - y);
		pastFlag = -1;	// Might not be needed
		valid = true;

		// Code for getting the executable path of the process behind the window
		DWORD dwProcId = 0;
		TCHAR cstrPath[MAX_PATH];
		GetWindowThreadProcessId(hwnd, &dwProcId);
		HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcId);
		GetModuleFileNameEx(hProc, NULL, cstrPath, MAX_PATH);
		wstring temp(cstrPath);
		windowModulePath = std::move(temp);
		if (!IsWindowVisible(winHandle)) {
			valid = false;
		}
	} catch (exception e) {
	#ifdef APPLICATION_DEBUG
		// Do some logging.
	#endif
		valid = false;
	}
}

Application::Application(Application&& app) noexcept {	
	hwnd = app.hwnd;
	x = std::move(app.x);
	y = std::move(app.y);
	width = std::move(app.width);
	depth = std::move(app.depth);
	pastFlag = std::move(app.pastFlag);
	windowModulePath = std::move(app.windowModulePath);
	valid = std::move(app.valid);
}

HWND Application::GetHWND() const {
	return hwnd;
}

int Application::GetX() const {
	return x;
}

int Application::GetY() const {
	return y;
}

int Application::GetWidth() const {
	return width;
}

int Application::GetDepth() const {
	return depth;
}

const UINT& Application::GetPastFlag() const {
	return pastFlag;
}

bool Application::IsValid() const {
	return valid;
}

const wstring& Application::GetWindowModulePath() const {
	return windowModulePath;
}

void Application::SetPosition(int x, int y, int cx, int cy, UINT flags) {
	try {	// NOTE: exception handlers here do nothing (keeping it just in case).
		ShowWindow(hwnd, SW_SHOWNORMAL);	// So far both of these ShowWindow statements resolve buggyness
		SetWindowPos(hwnd, HWND_TOP, x, y, cx, cy, SWP_SHOWWINDOW | SWP_ASYNCWINDOWPOS | SWP_DRAWFRAME);// needs to change
		SetForegroundWindow(hwnd);
		ShowWindow(hwnd, SW_SHOW);	// This could be used for the hide function
	} catch (exception e) {	
	#ifdef APPLICATION_DEBUG
		// Do some logging.
	#endif
		cout << "ERROR" << endl;
		valid = false;
		return;
	}

}

void Application::MinimizeApplication() const {
	ShowWindow(hwnd, SW_MINIMIZE);
}

// We need to implement a hide all profiles function
Application& Application::operator=(Application&& app) noexcept {
	if (this != &app) {
		hwnd = app.hwnd;
		x = std::move(app.x);
		y = std::move(app.y);
		width = std::move(app.width);
		depth = std::move(app.depth);
		pastFlag = std::move(app.pastFlag);
		windowModulePath = std::move(app.windowModulePath);
		valid = std::move(app.valid);
	}
	return *this;
}

wstring Application::ToString() {
	IsStillValid();
	if (!valid) {
		return L"Invalid Application Window! \n";
	}
	wstring toReturn;
	toReturn += L"x: " + to_wstring(x) + L"\n";
	toReturn += L"y: " + to_wstring(y) + L"\n";
	toReturn += L"width: " + to_wstring(width) + L"\n";
	toReturn += L"depth: " + to_wstring(depth) + L"\n";
	toReturn += L"pastFlag: " + to_wstring(pastFlag) + L"\n";
	toReturn += L"Module Path: " + windowModulePath + L"\n";
	return toReturn;
}

void Application::PrintApplicaiton() {
	cout << "HWND: " << hwnd << endl;
	wcout << ToString() << endl;
}

bool Application::IsStillValid() {
	CheckValid();
	return valid;
}

void Application::CheckValid() {
	__try {
		CheckValidHelper();
	} __except (EXCEPTION_EXECUTE_HANDLER) 	{
		wcout << "Closed or bad Window" << endl;
		valid = false;
	}
}

void Application::CheckValidHelper() {
	Application temp(hwnd);
	if (!temp.IsValid()) {
		valid = false;
	} else if (temp.GetWindowModulePath() != windowModulePath) {
		valid = false;
	}
}



