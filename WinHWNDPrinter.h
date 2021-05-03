#pragma once
#include <Windows.h>

class WinHWNDPrinter {
public:
	static void PrintHWNDInfo();
private:
	static BOOL CALLBACK InfoProcHelper(_In_ HWND hwnd, LPARAM IGNORED);

	// Helpers For InfoProcHelper
	void static PrintWindowVis(_In_ HWND);
	void static PrintRect(_In_ HWND);
	void static PrintStyles(_In_ HWND);
	void static PrintModuleFile(_In_ HWND);
};

