#pragma once
#include <vector>
#include <Windows.h>
#include <psapi.h>

class ProcessInfo {
public:

	ProcessInfo();
	
	// Methods

	void PrintInfo() const;
	

private:
	// Variables 

	DWORD* pids;
	unsigned int size;

	// Helpers

	void PrintHWNDInfo(const DWORD&) const;
	void PrintModuleFileName(const DWORD&) const;




};

