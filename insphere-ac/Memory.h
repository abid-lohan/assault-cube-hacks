#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <vector>

namespace memory {
	void patchBytes(PVOID newBytes, PVOID oldBytes, unsigned int size) {
		DWORD oldProtect;
		VirtualProtect(oldBytes, size, PAGE_EXECUTE_READWRITE, &oldProtect);
		memcpy(oldBytes, newBytes, size);
		VirtualProtect(oldBytes, size, oldProtect, &oldProtect);
	}

	void nopBytes(PVOID oldBytes, unsigned int size) {
		DWORD oldProtect;
		VirtualProtect(oldBytes, size, PAGE_EXECUTE_READWRITE, &oldProtect);
		memset(oldBytes, 0x90, size);
		VirtualProtect(oldBytes, size, oldProtect, &oldProtect);
	}

	uintptr_t calculateMultiLevelPointers(uintptr_t pointer, std::vector<unsigned int> offsets) {
		uintptr_t finalAddress = pointer;

		for (unsigned int i = 0; i < offsets.size(); i++) {
			finalAddress = *(uintptr_t*)finalAddress;
			finalAddress += offsets[i];
		}

		return finalAddress;
	}
}