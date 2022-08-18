#include "pch.h"
#include "memory.h"

void patchBytes(BYTE* newBytes, BYTE* oldBytes, unsigned int size) {
	DWORD oldProtect;
	VirtualProtect(oldBytes, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(oldBytes, newBytes, size);
	VirtualProtect(oldBytes, size, oldProtect, &oldProtect);
}

void nopBytes(BYTE* oldBytes, unsigned int size) {
	DWORD oldProtect;
	VirtualProtect(oldBytes, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	memset(oldBytes, 0x90, size);
	VirtualProtect(oldBytes, size, oldProtect, &oldProtect);
}

uintptr_t calculatePointers(uintptr_t pointer, std::vector<unsigned int> offsets) {
	uintptr_t finalAddress = pointer;

	for (unsigned int i = 0; i < offsets.size(); i++) {
		finalAddress = *(uintptr_t*)finalAddress;
		finalAddress += offsets[i];
	}

	return finalAddress;
}

bool hook(BYTE* locToHook, BYTE* myFunc, uintptr_t size) {
	if (size <= 4) return false;

	DWORD oldProtect;
	VirtualProtect(locToHook, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	memset(locToHook, 0x90, size);

	uintptr_t hookOffset = myFunc - locToHook - 5;

	*locToHook = 0xE9; //jmp
	*(uintptr_t*)(locToHook + 1) = hookOffset;

	VirtualProtect(locToHook, size, oldProtect, &oldProtect);

	return true;
}

BYTE* trampHook(BYTE* locToHook, BYTE* myFunc, unsigned int size) {
	if (size <= 4) return 0;

	BYTE* gateway = (BYTE*)VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	memcpy_s(gateway, size, locToHook, size);

	uintptr_t gatewayOffset = locToHook - gateway - 5;

	*(gateway + size) = 0xE9;
	*(uintptr_t*)(gateway + size + 1) = gatewayOffset;

	hook(locToHook, myFunc, size);

	return gateway;
}