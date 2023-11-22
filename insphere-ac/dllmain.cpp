#include "Memory.h"
#include "Process.h"
#include "Offsets.h"

DWORD WINAPI hackThread(HMODULE hModule) {
    const uintptr_t client = (uintptr_t)GetModuleHandle(L"ac_client.exe");
    const uintptr_t* player = (uintptr_t*)(client + offset::player);

    if (!player) {
        FreeLibraryAndExitThread(hModule, 0);
        return 0;
    }

    bool godMode = false;
    bool incAmmo = false;

    while (!GetAsyncKeyState(VK_F9)) {
        Sleep(100);

        if (GetAsyncKeyState(VK_F1) & 1) {
            godMode = !godMode;

            if (!godMode) {
                *(int*)(*player + offset::health) = 100;
            }
        }

        if (godMode) {
            *(int*)(*player + offset::health) = 1337;
        }

        if (GetAsyncKeyState(VK_F2) & 1) {
            incAmmo = !incAmmo;

            if (incAmmo) {
                memory::patchBytes((PVOID)"\xFF\x00", (PVOID)(client + offset::decAmmo), 2);
                uintptr_t assault = *player + offset::assault;
                *(uintptr_t*)memory::calculateMultiLevelPointers(assault, offset::weaponMagazine) = 1337;
                uintptr_t pistol = *player + offset::pistol;
                *(uintptr_t*)memory::calculateMultiLevelPointers(pistol, offset::weaponMagazine) = 1337;
            }
            else {
                memory::patchBytes((PVOID)"\xFF\x08", (PVOID)(client + offset::decAmmo), 2);
                uintptr_t assault = *player + offset::assault;
                *(uintptr_t*)memory::calculateMultiLevelPointers(assault, offset::weaponMagazine) = 40;
                uintptr_t pistol = *player + offset::pistol;
                *(uintptr_t*)memory::calculateMultiLevelPointers(pistol, offset::weaponMagazine) = 50;
            }
        }
    }

    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        DisableThreadLibraryCalls(hModule);
        const auto thread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)hackThread, hModule, 0, nullptr);
        if (thread) CloseHandle(thread);
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

