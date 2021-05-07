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
			MoveInstruction(std::wstring&& path, unsigned int index, int xx, int yy, int ccx, int ccy) {
				filePath = std::move(path);
				appIndex = index;
				x = xx;
				y = yy;
				cx = ccx;
				cy = ccy;
			}
			std::wstring ToString();
		};

		std::vector<MoveInstruction> instructions;
		Profile() = delete;
		Profile(std::wifstream& constrainedFile, bool constrained);
		std::wstring ToString();
		//ReadProfileAdvanced(std::ifstream& constrainedFile);
	};

	static void RunInstruction(const Profile::MoveInstruction& instruction);

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
	*		- totalX: The total amount of X divisions of the screen for which the movement in based on
	*		- totalY: Same as above except vertical
	*		- startX: The left most starting border of the window
	*			- EX: The window being split into fourths would have the following starts	|0	|1	|2	|3	| 
	*		- startY: The same as startX except vertical
	*		- widthX: The amount of blocks that the window will take up going left to right
	*		- widthY: The amount of blocks that the window will take up going left tor right
	*		NOTE: A different file format will be used for custom profile that are more specifc than the above
	*/			

	static void ReadProfilesConstrained(const char* filePath);


	// Getter
	const Application::WinMap& GetWindowedApps();

	static void  RunProfile(unsigned int index);
	
	static void  PrintWindowedApps();

	static void PrintSizeOfWindowedApps();

	static void PrintProfiles();

};

