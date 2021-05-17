#pragma once
#include <Windows.h>
#include <Psapi.h>
#include <unordered_map>
#include <string>
#include <vector>


class Application {
private:
	HWND hwnd;	// Identifier of the window (doesn't have to be valid)

	int x;		// Left most point of the window (client coords)
	int y;		// Right most point of the window (client coords)
	int width;		// Width of an application window (pixels)
	int depth;		// Depth of an application window (pixels)

	UINT pastFlag;

	std::wstring windowModulePath;	// The .exe path of a Application

	bool valid;

	void CheckValid();
	void CheckValidHelper();
	
public:
	/* Constructors */

	Application() = delete;
	Application(_In_ HWND winHandle);
	Application(Application&& app) noexcept;

	/* Accessors */

	HWND GetHWND() const;
	int GetX() const;
	int GetY() const;
	int GetWidth() const;
	int GetDepth() const;
	const UINT& GetPastFlag() const;
	const std::wstring& GetWindowModulePath() const;
	bool IsValid() const;	
	
	/* Methods */

	// Used to validate each application after initialization 
	bool IsStillValid();




	// Wrapper Method for SetWindowPos(...)
	void SetPosition(int x, int y, int cx, int cy, UINT flags);

	Application& operator=(Application&& app) noexcept;

	// Hides The Window
	void HideWindow();

	//To String Method
	std::wstring ToString();

	void PrintApplicaiton();

};

