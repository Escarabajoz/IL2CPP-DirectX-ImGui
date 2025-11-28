#include "Utils/Utils.h"
#include "Utils/Backend/Backend.h"
#include "Cheat/Hooks/Hooks.h"
#include <iostream>
#include <io.h>
#include <fcntl.h>

#define QWORD int64_t
FILE* f;

// run it in Release | x64, already set up everything

QWORD WINAPI MainThread(LPVOID param)
{
	RunHooks.Load(f); // Load IL2CPP FIRST before anything else
	RunBackend.Load(); // load backend after IL2CPP is initialized

	while (!Utils::KeyPressed(VK_END)) // uninject
		std::this_thread::sleep_for(std::chrono::milliseconds(420)); // no cpu frying in my city

	RunHooks.Unload(f);
	RunBackend.Unload(); // unload everything
	return 0;
}

BOOL __stdcall DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		// Allocate console and log DLL injection
		AllocConsole();
		FILE* console_out;
		freopen_s(&console_out, "CONOUT$", "w", stdout);
		freopen_s(&console_out, "CONIN$", "r", stdin);
		std::cout << "[DLL] IL2CPP DirectX ImGui Base injected successfully!" << std::endl;
		
		HANDLE hMainThread = CreateThread(nullptr, 0, LPTHREAD_START_ROUTINE(MainThread), hModule, 0, nullptr);

		if (hMainThread)
			CloseHandle(hMainThread);
		break;
	case DLL_PROCESS_DETACH:
		std::cout << "[DLL] IL2CPP DirectX ImGui Base detached!" << std::endl;
		FreeConsole();
		break;
	}
	return TRUE;
}