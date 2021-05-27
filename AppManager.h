#pragma once

#include <Windows.h>
#include <vector>
#include "Application.h"
#include "Displays.h"
#include <string>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <queue>


class AppManager {
public:
	class Profile;

	typedef std::unordered_map<std::wstring, std::queue<unsigned int>> LaunchUpdateMap;	// Add a seperate function path for updating when there is a profile launch...
	typedef std::unordered_map<std::wstring, std::unordered_map<unsigned int, Application>> WinMap;
	typedef std::unordered_map<std::wstring, std::vector<Profile>> ModeMap;
	typedef std::unordered_map<HWND, std::pair<unsigned int, std::wstring>> HandleMap;
	typedef std::unordered_map<std::wstring,
		std::priority_queue<unsigned int, std::vector<unsigned int>,
		std::greater<unsigned int>>> UpdateMap;

	class Profile {		// Profile contains move instructions which are all run when a profile is run
	private:
		void ReadProfileConstrained(std::wifstream& constrainedFile);

	public:
		struct PreInstruction {		// Used to create a move instruction
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

		struct MoveInstruction {	// Used to set the position of a window
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
	};

	// Backend Initializer
	static void Initialize();

	// Initialiser Helpers



	static void GetAllWindowedApplications();

	static void UpdateAllWindowedApplications();



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
	*/

	static void ReadProfilesConstrained(const WCHAR* filePath);


	const WinMap& GetWindowedApps();

	static void  RunProfile(unsigned int index);

	static void  PrintWindowedApps();

	static void PrintSizeOfWindowedApps();

	static void PrintProfiles();

	static void ClearProfiles();

	static void LaunchProfile(unsigned int index);

private:
	
	static void ClearProfile(const Profile& profile);

	static void ReadModeData(std::wifstream&, std::queue<std::pair<std::wstring, unsigned int>>&);

	static void RunInstruction(const Profile::MoveInstruction& instruction);

	static BOOL CALLBACK WindowConstructor(_In_ HWND hwnd, LPARAM IGNORED);

	static void MarkWindowUpdates();

	static BOOL CALLBACK WindowUpdater(_In_ HWND hwnd, LPARAM IGNORED);

	static bool CheckValidInstruction(const Profile::MoveInstruction& instruction, std::unordered_map<unsigned int, Application>::iterator& toReturn);

	static void LaunchWindowFromMoveInstruction(const Profile::MoveInstruction& instruction);

	static WinMap windowedApps;														// Stores all of the applications
	static std::unordered_set<HWND> handleSet;										// Stores all of the valid handles
	static UpdateMap updateMap;														// Stores stores indices of applications that need to be updated
	static ModeMap modes;											// Stores all profiles associated with their modes
	static MONITORINFO monitorInfo;													// Stores the info of a monitor
	static std::unordered_map <std::wstring, unsigned int> constructionIndexes;
	static std::wstring currentMode;
	static LaunchUpdateMap launchUpdateMap;
};

