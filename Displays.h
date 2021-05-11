#pragma once

#include <Windows.h>
#include <vector>
#include <fstream>
#include <string>



struct Displays {
public:
	struct Display {
		HMONITOR handle;
		RECT workArea;
		Display(HMONITOR h, RECT area) : handle(h), workArea(area) {};
		std::wstring ToString();
	};

	// Finds all the dispalys that are connected
	static void Initialize();

	static std::vector<Display> displays;

	/* Writes to a file that will be used by the front end
	*	- This file will be updated every time the user launches the front end of the application
	*	- This will determine what the Profile selection and edit screens will look like
	*	- This file will should be read during the loading faze of the application
	*/
	
	//static void WriteDisplays(); later displays are for now assumed to be 1920 by 1080

	static std::wstring ToString();

private:
	
	
	static BOOL CALLBACK FindMonitor(HMONITOR, HDC, LPRECT, LPARAM);



};

