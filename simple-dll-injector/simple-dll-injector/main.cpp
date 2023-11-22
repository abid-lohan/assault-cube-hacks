#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

DWORD getProcessID(const char* procName) {
	DWORD procID = 0;
	HANDLE snapHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (snapHandle != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(snapHandle, &procEntry)) {
			do {
				if (!_stricmp(procEntry.szExeFile, procName)) {
					procID = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(snapHandle, &procEntry));
		}
	}

	CloseHandle(snapHandle);
	return procID;
}

int main() {
	const char* pathToDLL = "C:/insphere-ac.dll"; // Change the DLL here (full path)
	DWORD procID = 0;

	while (!procID) {
		procID = getProcessID("ac_client.exe"); // Change the game here
		Sleep(30);
	}

	std::cout << "Process ID: " << procID << std::endl;

	HANDLE procHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, procID);

	if (procHandle && procHandle != INVALID_HANDLE_VALUE) {
		void* loc = VirtualAllocEx(procHandle, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		std::cout << "Memory location: " << loc << std::endl;

		if (loc) WriteProcessMemory(procHandle, loc, pathToDLL, strlen(pathToDLL) + 1, 0);

		HANDLE threadHandle = CreateRemoteThread(procHandle, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);

		if (threadHandle){
			std::cout << "Remote Thread successfully created." << std::endl;
			CloseHandle(threadHandle);
		}
	}

	if (procHandle) CloseHandle(procHandle);

	std::cout << "Process Handle and Thread Handle closed." << std::endl;
	std::cout << "READY!" << std::endl;
	std::cin.get();

	return 0;
}