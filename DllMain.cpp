#include "Utils/Utils.h"
#include "Utils/Backend/Backend.h"
#include "Cheat/Hooks/Hooks.h"

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
	case 1:
		HANDLE hMainThread = CreateThread(nullptr, 0, LPTHREAD_START_ROUTINE(MainThread), hModule, 0, nullptr);

		if (hMainThread)
			CloseHandle(hMainThread);
		break;
	}
	return TRUE;
}