#pragma once

#include <Windows.h>
#include <TlHelp32.h>

DWORD getProcessID(const wchar_t* procName);

uintptr_t getModuleBaseAddress(DWORD procID, const wchar_t* modName);