#include "Memory.h"

void patchBytesEx(BYTE* newBytes, BYTE* oldBytes, unsigned int size, HANDLE procHandle) {
	DWORD oldProtect;
	VirtualProtect(oldBytes, size, PAGE_EXECUTE_READWRITE, &oldProtect);

	WriteProcessMemory(procHandle, oldBytes, newBytes, size, nullptr);

	VirtualProtect(oldBytes, size, oldProtect, &oldProtect);
}

void nopBytesEx(BYTE* oldBytes, unsigned int size, HANDLE procHandle) {
	BYTE* nopper = new BYTE[size];
	memset(nopper, 0x90, size);

	patchBytesEx(nopper, oldBytes, size, procHandle);
	delete[] nopper;
}

uintptr_t calculatePointersEx(uintptr_t pointer, std::vector<unsigned int> offsets, HANDLE procHandle) {
	uintptr_t finalAddress = pointer;

	for (unsigned int i = 0; i < offsets.size(); i++) {
		ReadProcessMemory(procHandle, (BYTE*)finalAddress, &finalAddress, sizeof(finalAddress), 0);
		finalAddress += offsets[i];
	}

	return finalAddress;
}