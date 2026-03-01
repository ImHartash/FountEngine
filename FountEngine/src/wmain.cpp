#include <Windows.h>
#include "engine/application/CApplication.hpp"

int WINAPI WinMain(
	_In_ HINSTANCE hInstance, 
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PSTR pCmdLine, 
	_In_ int nCmdShow) 
{
	CApplication App;
	if (!App.Initialize(hInstance, L"FountEngine Test"))
		return -1;

	return App.Run();
}