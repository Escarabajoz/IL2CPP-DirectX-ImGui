#include "Hooks.h"
#include "../Functions/Functions.h"

Hooks RunHooks;
Signatures RunSignatures;

void Hooks::Load(FILE* f)
{
	Hooks::LoadConsole(f); // need to put this first in case of melonloader junking your console
	Hooks::LoadModules();
	// this function calls when ur injecting ur dll
	RunSignatures.SearchSignatures(true); // true, so you getting also the addresses to your console
	//Hooks::LoadDetourHooks(); // if u using signatures like in the example, first let run the searchsignatures function, only uncomment if u wanna run all the functions when injecting dll
	//Hooks::LoadMinHookHooks(); // @TODO: UNCOMMENT ME IF U WANNA USE ME 
}

void Hooks::LoadModules()
{
	if (IL2CPP::Initialize(true))
	{
		UnitySDK::UnityGameBase = (uintptr_t)GetModuleHandleA(NULL);
		printf("[ @zzzmate ] Base Address: 0x%llX\n", UnitySDK::UnityGameBase);
		UnitySDK::UnityGameAssembly = (uintptr_t)GetModuleHandleA("GameAssembly.dll");
		printf("[ @zzzmate ] GameAssembly Base Address: 0x%llX\n", UnitySDK::UnityGameAssembly);
		UnitySDK::UnityPlayer = (uintptr_t)GetModuleHandleA("UnityPlayer.dll");
		printf("[ @zzzmate ] UnityPlayer Base Address: 0x%llX\n", UnitySDK::UnityPlayer);
		
		printf("[ @zzzmate ] IL2CPP::Initialize SUCCESS\n");
	}
	else
	{
		printf("[ @zzzmate ] ERROR: IL2CPP::Initialize FAILED!\n");
		return;
	}

	IL2CPP::Callback::Initialize();
	printf("[ @zzzmate ] IL2CPP::Callback::Initialize SUCCESS\n");
}

void Hooks::LoadConsole(FILE* f)
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	SetConsoleTitle(L"@zzzmate");
	freopen_s(&f, "CONOUT$", "w", stdout);
	system("cls"); // if you using melonloader you gon need this
}

void Hooks::LoadDetourHooks()
{
	// Camera hook initialization - verify camera is accessible
	Unity::CCamera* testCamera = Unity::Camera::GetMain();
	if (testCamera != nullptr)
	{
		printf("[ @zzzmate ] Camera::GetMain() initialized successfully at 0x%p\n", testCamera);
	}
	else
	{
		printf("[ @zzzmate ] WARNING: Camera::GetMain() returned nullptr\n");
	}
}

void Hooks::LoadMinHookHooks()
{
	// Add your MinHook hooks here
}

void Hooks::UnloadMinHookHooks()
{
	// Unload your MinHook hooks here
	// don't uninitaliaze, backend going to do that!!!
}

void Hooks::UnloadConsole(FILE* f)
{
	FreeConsole();
	if (f) fclose(f);
}

void Hooks::Unload(FILE* f)
{
	//Hooks::UnloadMinHookHooks(); // @TODO: UNCOMMENT ME IF U USING ME!

	IL2CPP::Callback::Uninitialize();
	Hooks::UnloadConsole(f);
	// don't uninitaliaze, backend going to do that!!!
}