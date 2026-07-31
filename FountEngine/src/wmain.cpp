#include <Windows.h>
#include "engine/application/CApplication.hpp"
#include "systems/CSystemManager.hpp"
#include "systems/logsystem/CLogSystem.hpp"

int WINAPI WinMain(
	_In_ HINSTANCE hInstance, 
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PSTR pCmdLine, 
	_In_ int nCmdShow) 
{
	if (!CSystemManager::GetInstance().Initialize()) {
		MessageBox(0, L"Failed to initialize CSystemManager!", L"Initialization Error", 0);
		return -1;
	}

	if (!g_pLogSystem->Initialize("FountEngine_Latest.log")) {
		MessageBox(0, L"Failed to initialize CLogSystem!", L"Initialization Error", 0);
		return -1;
	}
	
	LOG_INFO("LogSystem initialized successfully!");
	LOG_DEBUG("Initializing Application...");

	CApplication App;
	if (!App.Initialize(hInstance, L"FountEngine Test")) {
		LOG_FATAL("Failed to initialize application!");
		return -1;
	}

	LOG_INFO("Successfully initialized application!");

	int nResult = App.Run();
	LOG_DEBUG("Application shutdown with code %d", nResult);

	return nResult;
}