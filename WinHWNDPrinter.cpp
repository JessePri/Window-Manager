#include "WinHWNDPrinter.h"
#include <Windows.h>
#include <Psapi.h>
#include <iostream>
#define UNICODE

using std::cout;
using std::endl;
using std::boolalpha;
using std::hex;
using std::wprintf;

void PrintLPSTR(LPSTR charStr) {
	for (unsigned int i = 0; charStr[i] != '\0'; ++i) {
		cout << charStr[i];
	}
}

void PrintLPWSTR(LPWSTR charStr) {
	wprintf(charStr);
}

//BOOL WinHWNDPrinter::InfoProcHelper(_In_ HWND hwnd, _In_ LPARAM IGNORED) {
//	cout << "------------------------------------------" << endl;
//	cout << "Handle: " << hwnd << endl;
//	cout << "IsWindow: " << IsWindow(hwnd) << endl;
//	cout << "IsVisibleWindow: " << IsWindowVisible(hwnd) << endl;
//	cout << endl;
//
//	RECT rect;
//	GetWindowRect(hwnd, &rect);
//	cout << "Rect Left: " << rect.left << endl;
//	cout << "Rect Right: " << rect.right << endl;
//	cout << "Rect Top: " << rect.top << endl;
//	cout << "Rect Bottom: " << rect.bottom << endl;
//
//	WINDOWINFO winfo;
//	GetWindowInfo(hwnd, &winfo);
//	cout << "Window Styles: " << hex << winfo.dwStyle << endl;
//	cout << "Window Extra Styles: " << hex << winfo.dwExStyle << endl;
//
//	const unsigned int maxSize = 400;
//	char fileName[maxSize] = {};
//	GetWindowModuleFileNameA(hwnd, fileName, maxSize);
//	cout << "Module File: ";
//	PrintLPSTR(fileName);
//	cout << endl;
//	LONG_PTR handle = GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
//	cout << endl;
//
//	WCHAR fileName2[maxSize] = {};
//	GetModuleFileNameEx((HANDLE)handle, NULL, fileName2, maxSize);
//	cout << "Modeule file 2: ";
//	PrintLPWSTR(fileName2);
//	cout << endl;
//
//	TCHAR buffer[MAX_PATH] = { 0 };
//	DWORD dwProcId = 0;
//
//	GetWindowThreadProcessId(hwnd, &dwProcId);
//
//	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcId);
//	GetModuleFileNameEx(hProc, NULL, buffer, MAX_PATH);
//	cout << "Copy Pasta FileName: ";
//	PrintLPWSTR(buffer);
//	cout << endl;
//	CloseHandle(hProc);
//	
//	cout << "------------------------------------------" << endl;
//	return true;
//
//}

BOOL WinHWNDPrinter::InfoProcHelper(_In_ HWND hwnd, _In_ LPARAM IGNORED) {	
	if (IsWindowVisible(hwnd) == 0) {
		return true;
	}
	cout << "------------------------------------------" << endl;
	PrintWindowVis(hwnd);
	PrintRect(hwnd);
	PrintStyles(hwnd);
	PrintModuleFile(hwnd);
	cout << "------------------------------------------" << endl;
	return true;
}


void WinHWNDPrinter::PrintWindowVis(_In_ HWND hwnd) {
	cout << "Handle: " << hwnd << endl;
	cout << "IsWindow: " << IsWindow(hwnd) << endl;
	cout << "IsVisibleWindow: " << IsWindowVisible(hwnd) << endl;
	cout << endl;
}

void WinHWNDPrinter::PrintRect(_In_ HWND hwnd) {
	RECT rect;
	GetWindowRect(hwnd, &rect);
	cout << "Rect Left: " << rect.left << endl;
	cout << "Rect Right: " << rect.right << endl;
	cout << "Rect Top: " << rect.top << endl;
	cout << "Rect Bottom: " << rect.bottom << endl;
	cout << endl;
}

void WinHWNDPrinter::PrintStyles(_In_ HWND hwnd) {
	WINDOWINFO winfo;
	GetWindowInfo(hwnd, &winfo);
	cout << "Window Styles: " << hex << winfo.dwStyle << endl;
	cout << "Window Extra Styles: " << hex << winfo.dwExStyle << endl;
	cout << endl;
}

void WinHWNDPrinter::PrintModuleFile(_In_ HWND hwnd) {
	TCHAR buffer[MAX_PATH] = { 0 };
	DWORD dwProcId = 0;

	GetWindowThreadProcessId(hwnd, &dwProcId);

	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcId);
	GetModuleFileNameEx(hProc, NULL, buffer, MAX_PATH);
	cout << "Copy Pasta FileName: ";
	PrintLPWSTR(buffer);
	cout << endl;
	CloseHandle(hProc);
	cout << endl;
}











void WinHWNDPrinter::PrintHWNDInfo() {
	EnumWindows(&InfoProcHelper, 0);
}



