#pragma once

#include <Windows.h>
#include <vector>

void patchBytes(BYTE* newBytes, BYTE* oldBytes, unsigned int size);

void nopBytes(BYTE* oldBytes, unsigned int size);

uintptr_t calculatePointers(uintptr_t pointer, std::vector<unsigned int> offsets);

bool hook(BYTE* locToHook, BYTE* myFunc, unsigned int size);

BYTE* trampHook(BYTE* locToHook, BYTE* myFunc, unsigned int size);