#pragma once

#include <Windows.h>
#include <vector>
#include "Application.h"
#include "Displays.h"
#include <string>
#include <fstream>


class AppManager {
private:

	class Profile {
	private:
		void ReadProfileConstrained(std::wifstream& constrainedFile);

	public:
		struct PreInstruction {
			std::wstring filePath = L"";
			unsigned int appIndex = 0;
			unsigned int displayID = 0;
			double totalX = 0;
			double totalY = 0;
			double startX = 0;
			double startY = 0;
			double widthX = 0;
			double widthY = 0;
		};

		struct MoveInstruction {
			std::wstring filePath;
			unsigned int appIndex;
			int x;
			int y;
			int cx;
			int cy;
			MoveInstruction() {
				WCHAR c = '0';
				filePath = c;
				appIndex = 0;
				x = 0;
				y = 0;
				cx = 0;
				cy = 0;
			}
			MoveInstruction(const PreInstruction& preInstruction);
			std::wstring ToString() const;
		};

		

		std::vector<MoveInstruction> instructions;
		Profile() = delete;
		Profile(std::wifstream& constrainedFile, bool constrained);
		std::wstring ToString();
		//ReadProfileAdvanced(std::ifstream& constrainedFile);
	};

	static void RunInstruction(const Profile::MoveInstruction& instruction);

	static void CreateNewWindow(const AppManager::Profile::MoveInstruction& instruction);

	static BOOL CALLBACK FindNewValidWindow(_In_ HWND hwnd, LPARAM);
	static Application newValidWindow;
	static std::wstring modulePathToCompare;

	static Application::WinMap windowedApps;
	static std::vector<Profile> profiles;
	static MONITORINFO monitorInfo;

public:

	// Backend Initializer
	static void Initialize();

	// Initialiser Helpers


	
	static void GetAllWindowedApplications();
	static BOOL CALLBACK WindowConstructor(_In_ HWND hwnd, LPARAM IGNORED);

	// File Readers

	/* Constrained File Format:
	*	- So far the file format will support halfs, thirds and fourths 
	*	- The file format for each process which is to be loaded at the start of the front end would be as follows
	*		- exePath: The path of the exe of the application (the user should give us this in the GUI application)
	*		- identifier: The number of the app (identifies two seperate apps)
	*		- display identifier: The index of the display inside of displays
	*		- totalX: The total amount of X divisions of the screen for which the movement in based on
	*		- totalY: Same as above except vertical
	*		- startX: The left most starting border of the window
	*			- EX: The window being split into fourths would have the following starts	|0	|1	|2	|3	| 
	*		- startY: The same as startX except vertical
	*		- widthX: The amount of blocks that the window will take up going left to right
	*		- widthY: The amount of blocks that the window will take up going left tor right
	*		NOTE: A different file format will be used for custom profile that are more specifc than the above
	*/			

	static void ReadProfilesConstrained(const WCHAR* filePath);


	const Application::WinMap& GetWindowedApps();

	static void  RunProfile(unsigned int index);
	
	static void  PrintWindowedApps();

	static void PrintSizeOfWindowedApps();

	static void PrintProfiles();

};

