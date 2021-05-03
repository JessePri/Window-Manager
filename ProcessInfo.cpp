#include "ProcessInfo.h"
#include <iostream>
#include <vector>
#include <Windows.h>
#include <Psapi.h>

using std::vector;
using std::cout;
using std::endl;
using std::wprintf;

ProcessInfo::ProcessInfo() {
	const int maxSize = 400;
	unsigned long sizeUsed;
	pids = new DWORD[maxSize];
	EnumProcesses(pids, maxSize, &sizeUsed);
	size = sizeUsed / (sizeof(unsigned long));
}

void ProcessInfo::PrintInfo() const {
	for (unsigned int i = 0; i < size; ++i) {
		PrintModuleFileName(pids[i]);
		//PrintHWNDInfo(pids[i]);
	}
}

void ProcessInfo::PrintModuleFileName(const DWORD& pid) const {
	TCHAR buffer[MAX_PATH] = { 0 };
	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | READ_CONTROL, FALSE, pid);
	GetModuleFileName((HMODULE)hProc, buffer, MAX_PATH);
	cout << "Copy Pasta FileName: ";
	wprintf(buffer);
	cout << endl;
	CloseHandle(hProc);

	cout << "------------------------------------------" << endl;
}

void ProcessInfo::PrintHWNDInfo(const DWORD& pid) const {
	DWORD toPrint;
	//GetWindowThreadProcessId()
}