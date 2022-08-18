#include <iostream>
#include "process.h"
#include "memory.h"

class Player {
private:
	std::vector<unsigned int> currentWeaponAmmoOffsets = { 0x150 };
	std::vector<unsigned int> healthOffsets = { 0xF8 };
	std::vector<unsigned int> assaultDPSOffsets = { 0x360, 0xC, 0x10A };

public:
	uintptr_t healthAddr = 0;
	uintptr_t currentWeaponAmmoAddr = 0;
	uintptr_t assaultDPSAddr = 0;
	uintptr_t decAmmoAddr = 0;
	uintptr_t damageAddr = 0;
	uintptr_t recoilAddr = 0;
	uintptr_t playerAddr = 0; 

	Player(uintptr_t modBaseAddress, HANDLE& procHandle) {
		this->playerAddr = modBaseAddress + 0x109B74;
		this->currentWeaponAmmoAddr = calculatePointersEx(this->playerAddr, this->currentWeaponAmmoOffsets, procHandle);
		this->healthAddr = calculatePointersEx(this->playerAddr, this->healthOffsets, procHandle);
		this->assaultDPSAddr = calculatePointersEx(this->playerAddr, this->assaultDPSOffsets, procHandle);
		this->decAmmoAddr = modBaseAddress + 0x637E9;
		this->damageAddr = modBaseAddress + 0x2CA5E;
		this->recoilAddr = modBaseAddress + 0x63786;
	}
};

int main() {
	// Proccess ID -> Module Base Address
	DWORD procID = 0;
	uintptr_t modBaseAddress = 0;
	procID = getProcessID(L"ac_client.exe");
	modBaseAddress = getModuleBaseAddress(procID, L"ac_client.exe");

	std::cout << "Welcome to Sphere!" << std::endl << "A Assault Cube External Cheat made by Ruhptura." << std::endl;
	std::cout << "--------------------------------------------" << std::endl;

	if (!procID || !modBaseAddress) {
		std::cout << "Process ID or module not found." << std::endl << "Press any key to exit." << std::endl;
		std::cin.get();
		return 0;
	}

	// Handle Proccess

	HANDLE procHandle = 0;
	procHandle = OpenProcess(PROCESS_ALL_ACCESS, NULL, procID);

	// Base Address of Pointer Chain -> Address of Interest
	
	Player player(modBaseAddress, procHandle);
	unsigned int ammo = 0;
	unsigned int health = 0;
	unsigned int assaultDPS = 0;

	if (player.healthAddr && player.currentWeaponAmmoAddr && player.assaultDPSAddr) {
		std::cout << "Player found!" << std::endl;
		std::cout << "Ammo address: 0x" << std::hex << player.currentWeaponAmmoAddr << std::endl;
		std::cout << "Health address: 0x" << std::hex << player.healthAddr << std::endl;
		std::cout << "Assault DPS address: 0x" << std::hex << player.assaultDPSAddr << std::endl;
		std::cout << "--------------------------------------------" << std::endl;

		ReadProcessMemory(procHandle, (BYTE*)player.currentWeaponAmmoAddr, &ammo, sizeof(ammo), nullptr);
		ReadProcessMemory(procHandle, (BYTE*)player.healthAddr, &health, sizeof(health), nullptr);
		ReadProcessMemory(procHandle, (BYTE*)player.assaultDPSAddr, &assaultDPS, sizeof(assaultDPS), nullptr);

		std::cout << "Ammo: " << std::dec << ammo << std::endl;
		std::cout << "Health: " << std::dec << health << std::endl;
		std::cout << "Assault DPS: " << std::dec << assaultDPS << std::endl;
		std::cout << "--------------------------------------------" << std::endl;

		std::cout << "[?] F1: God Mode" << std::endl;
		std::cout << "[?] F2: Increment Ammo" << std::endl;
		std::cout << "[?] F3: High DPS" << std::endl;
		std::cout << "[?] F4: No Recoil" << std::endl;
		std::cout << "[?] F9: Close Sphere" << std::endl;
		std::cout << "--------------------------------------------" << std::endl;
	}

	// Cheat logic

	DWORD exitStatus = 0;
	bool ammoActive = false;
	bool godActive = false;
	bool dpsActive = false;
	bool recoilActive = false;
	unsigned int newHealth = 1337;

	while (GetExitCodeProcess(procHandle, &exitStatus) && exitStatus == STILL_ACTIVE) {
		if (GetAsyncKeyState(VK_F1) & 1) { //HP
			godActive = !godActive;

			if (godActive) {
				WriteProcessMemory(procHandle, (BYTE*)player.healthAddr, &newHealth, sizeof(newHealth), nullptr);

				std::cout << "[+] God Mode Activated!" << std::endl;
			}
			else {
				unsigned int normalHealth = 100;
				WriteProcessMemory(procHandle, (BYTE*)player.healthAddr, &normalHealth, sizeof(normalHealth), nullptr);

				std::cout << "[-] God Mode Deactivated!" << std::endl;
			}

		}

		if (GetAsyncKeyState(VK_F2) & 1) { //Ammo
			if (!ammoActive) {
				patchBytesEx((BYTE*)"\xFF\x06", (BYTE*)player.decAmmoAddr, 2, procHandle);
				ammoActive = true;

				std::cout << "[+] Increment Ammo Activated!" << std::endl;
			}
			else {
				patchBytesEx((BYTE*)"\xFF\x0E", (BYTE*)player.decAmmoAddr, 2, procHandle);
				ammoActive = false;

				std::cout << "[-] Increment Ammo Deactivated!" << std::endl;
			}
		}

		if (GetAsyncKeyState(VK_F3) & 1) { //DPS

			dpsActive = !dpsActive;

			if (dpsActive) {
				unsigned int dps = 0;
				WriteProcessMemory(procHandle, (BYTE*)player.assaultDPSAddr, &dps, 2, nullptr);

				std::cout << "[+] High DPS Activated!" << std::endl;
			}
			else {
				unsigned int dps = 120;
				WriteProcessMemory(procHandle, (BYTE*)player.assaultDPSAddr, &dps, 2, nullptr);

				std::cout << "[-] High DPS Deactivated!" << std::endl;
			}
		}

		if (GetAsyncKeyState(VK_F4) & 1) { //Recoil
			if (!recoilActive) {
				recoilActive = true;

				nopBytesEx((BYTE*)player.recoilAddr, 10, procHandle);

				std::cout << "[+] No Recoil Activated!" << std::endl;
			}
			else {
				recoilActive = false;

				patchBytesEx((BYTE*)"\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2", (BYTE*)player.recoilAddr, 10, procHandle);

				std::cout << "[-] No Recoil Deactivated!" << std::endl;
			}
		}

		if (GetAsyncKeyState(VK_F9) & 1) { //Exit
			return 0;
		}

		if (godActive) {
			WriteProcessMemory(procHandle, (BYTE*)player.healthAddr, &newHealth, sizeof(newHealth), nullptr);
			Sleep(100);
		}
	}

	std::cin.get();
}