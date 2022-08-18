#include <iostream>
#include "pch.h"
#include "process.h"
#include "memory.h"

class Player {
private:
	std::vector<unsigned int> currentWeaponAmmoOffsets = { 0x150 };
	std::vector<unsigned int> healthOffsets = { 0xF8 };
	std::vector<unsigned int> assaultDPSOffsets = { 0x360, 0xC, 0x10A };

public:
	uintptr_t playerAddr = 0;
	uintptr_t healthAddr = 0;
	uintptr_t currentWeaponAmmoAddr = 0;
	uintptr_t assaultDPSAddr = 0;
	uintptr_t decAmmoAddr = 0;
	uintptr_t damageAddr = 0;
	uintptr_t recoilAddr = 0;

	Player(uintptr_t modBaseAddress) {
		this->playerAddr = modBaseAddress + 0x109B74;
		this->currentWeaponAmmoAddr = calculatePointers(playerAddr, currentWeaponAmmoOffsets);
		this->healthAddr = calculatePointers(playerAddr, healthOffsets);
		this->assaultDPSAddr = calculatePointers(playerAddr, assaultDPSOffsets);
		this->decAmmoAddr = modBaseAddress + 0x637E9;
		this->damageAddr = modBaseAddress + 0x2CA5E;
		this->recoilAddr = modBaseAddress + 0x63786;
	}
};

typedef BOOL(__stdcall* twglSwapBuffers) (HDC hDc);
twglSwapBuffers owglSwapBuffers;

uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");
Player player(moduleBase);

bool ammoActive = false;
bool godActive = false;
bool dpsActive = false;
bool recoilActive = false;
unsigned int newHealth = 1337;
unsigned int normalHealth = 100;

BOOL __stdcall hacks(HDC hDc) {
	if (GetAsyncKeyState(VK_F1) & 1) { //HP
		godActive = !godActive;

		if (godActive) {
			memcpy((BYTE*)player.healthAddr, &newHealth, sizeof(newHealth));
		}
		else {
			memcpy((BYTE*)player.healthAddr, &normalHealth, sizeof(normalHealth));
		}

	}

	if (GetAsyncKeyState(VK_F2) & 1) { //Ammo
		if (!ammoActive) {
			patchBytes((BYTE*)"\xFF\x06", (BYTE*)player.decAmmoAddr, 2);
			ammoActive = true;
		}
		else {
			patchBytes((BYTE*)"\xFF\x0E", (BYTE*)player.decAmmoAddr, 2);
			ammoActive = false;
		}
	}

	if (GetAsyncKeyState(VK_F3) & 1) { //DPS

		dpsActive = !dpsActive;

		if (dpsActive) {
			unsigned int dps = 0;
			memcpy((BYTE*)player.assaultDPSAddr, &dps, 2);
		}
		else {
			unsigned int dps = 120;
			memcpy((BYTE*)player.assaultDPSAddr, &dps, 2);
		}
	}

	if (GetAsyncKeyState(VK_F4) & 1) { //Recoil

		recoilActive = !recoilActive;

		if (recoilActive) {
			nopBytes((BYTE*)player.recoilAddr, 10);
		}
		else {
			patchBytes((BYTE*)"\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2", (BYTE*)player.recoilAddr, 10);
		}
	}

	if (godActive) {
		memcpy((BYTE*)player.healthAddr, &newHealth, sizeof(newHealth));
	}

	return owglSwapBuffers(hDc);
}

DWORD WINAPI hackThread(HMODULE hModule) {
	// Spawn console
	AllocConsole();
	FILE* fConsole;
	freopen_s(&fConsole, "CONOUT$", "w", stdout);

	// Get DLL function address for hooking

	HMODULE hDLL = GetModuleHandle(L"opengl32.dll");

	if (hDLL) owglSwapBuffers = (twglSwapBuffers)GetProcAddress(hDLL, "wglSwapBuffers");

	owglSwapBuffers = (twglSwapBuffers)trampHook((BYTE*)owglSwapBuffers, (BYTE*)hacks, 5); //gateway

	// Cleanup and eject

	fclose(fConsole);
	FreeConsole();
	//FreeLibraryAndExitThread(hModule, 0);

	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: {
		CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)hackThread, hModule, 0, nullptr));
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

