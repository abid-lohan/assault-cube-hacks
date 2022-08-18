#pragma once

#include <Windows.h>
#include <vector>

void patchBytesEx(BYTE* newBytes, BYTE* oldBytes, unsigned int size, HANDLE procHandle);

void nopBytesEx(BYTE* oldBytes, unsigned int size, HANDLE procHandle);

uintptr_t calculatePointersEx(uintptr_t pointer, std::vector<unsigned int> offsets, HANDLE procHandle);