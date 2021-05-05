#include "Application.h"
#include <exception>
#include <Windows.h>
#include <Psapi.h>
#include <string>


using std::wstring;
using std::exception;
using std::to_wstring;

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
	try {
		SetWindowPos(hwnd, HWND_TOP, x, y, cx, cy, flags);
	} catch (exception e) {
	#ifdef APPLICATION_DEBUG
		// Do some logging.
	#endif
		valid = false;
		return;
	}
	
}

void Application::HideWindow() {
	try {
		SetWindowPos(hwnd, HWND_TOP, x, y, width, depth, SWP_HIDEWINDOW);
	} catch (exception e) {
	#ifdef APPLICATION_DEBUG
		// Do some logging.
	#endif
		valid = false;
		return;
	}
}

Application& Application::operator=(Application&& app) noexcept {
	if (this != &app) {
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
	if (!valid) {
		return L"Invalid Application Window! \n";
	}
	wstring toReturn;
	toReturn += L"HWND: " + to_wstring((unsigned long)hwnd) + L"\n";
	toReturn += L"x: " + to_wstring(x) + L"\n";
	toReturn += L"y: " + to_wstring(y) + L"\n";
	toReturn += L"width: " + to_wstring(width) + L"\n";
	toReturn += L"depth: " + to_wstring(depth) + L"\n";
	toReturn += L"pastFlag: " + to_wstring(pastFlag) + L"\n";
	toReturn += L"Module Path: " + windowModulePath + L"\n";
	return toReturn;
}

