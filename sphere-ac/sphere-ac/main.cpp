#include <iostream>
#include "process.h"
#include "memory.h"

class Player {
private:
	std::vector<unsigned int> currentWeaponOffsets = { 0x364 };
	std::vector<unsigned int> assaultOffsets = { 0x354 };
	std::vector<unsigned int> pistolOffsets = { 0x340 };
	std::vector<unsigned int> weaponAmmoOffsets = { 0x14, 0x0 };
	std::vector<unsigned int> weaponMagazineOffsets = { 0x10, 0x0 };
	std::vector<unsigned int> healthOffsets = { 0xEC };

public:
	uintptr_t healthAddr;
	uintptr_t assaultAddr;
	uintptr_t assaultAmmoAddr;
	uintptr_t assaultMagazineAddr;
	uintptr_t pistolAddr;
	uintptr_t pistolAmmoAddr;
	uintptr_t pistolMagazineAddr;
	uintptr_t decAmmoAddr;
	uintptr_t playerAddr;
	unsigned int health;

	Player(uintptr_t modBaseAddress, HANDLE& procHandle) {
		this->playerAddr = modBaseAddress + 0x17E0A8;
		this->decAmmoAddr = modBaseAddress + 0xC73EF;
		this->assaultAddr = memory::calculateMultiLevelPointers(this->playerAddr, this->assaultOffsets, procHandle);
		this->pistolAddr = memory::calculateMultiLevelPointers(this->playerAddr, this->pistolOffsets, procHandle);

		this->assaultAmmoAddr = memory::calculateMultiLevelPointers(this->assaultAddr, this->weaponAmmoOffsets, procHandle);
		this->assaultMagazineAddr = memory::calculateMultiLevelPointers(this->assaultAddr, this->weaponMagazineOffsets, procHandle);
		this->pistolAmmoAddr = memory::calculateMultiLevelPointers(this->pistolAddr, this->weaponAmmoOffsets, procHandle);
		this->pistolMagazineAddr = memory::calculateMultiLevelPointers(this->pistolAddr, this->weaponMagazineOffsets, procHandle);
		this->healthAddr = memory::calculateMultiLevelPointers(this->playerAddr, this->healthOffsets, procHandle);
		
		ReadProcessMemory(procHandle, (PVOID)this->healthAddr, &this->health, sizeof(this->health), nullptr);
	}
};

int main() {
	DWORD procID = process::getProcessID(L"ac_client.exe");
	uintptr_t modBaseAddress = process::getModuleBaseAddress(procID, L"ac_client.exe");

	std::cout << "Welcome to Sphere!" << std::endl << "An Assault Cube External Cheat made by ruhptura." << std::endl;
	std::cout << "--------------------------------------------" << std::endl;

	if (!procID || !modBaseAddress) {
		std::cout << "Process ID or module not found." << std::endl << "Press any key to exit." << std::endl;
		std::cin.get();
		return 0;
	}

	HANDLE procHandle = OpenProcess(PROCESS_ALL_ACCESS, NULL, procID);
	Player player(modBaseAddress, procHandle);

	if (!player.healthAddr) {
		std::cout << "Player not found." << std::endl << "Press any key to exit." << std::endl;
		std::cin.get();
		return 0;
	}

	std::cout << "Player found!" << std::endl;

	std::cout << "Health: " << std::dec << player.health << std::endl;
	std::cout << "--------------------------------------------" << std::endl;

	std::cout << "[i] F1: God Mode ON/OFF" << std::endl;
	std::cout << "[i] F2: Increment Ammo ON/OFF" << std::endl;
	std::cout << "[i] F9: Close Sphere" << std::endl;
	std::cout << "--------------------------------------------" << std::endl;

	DWORD exitStatus = 0;
	bool ammoActive = false;
	bool godActive = false;
	bool dpsActive = false;
	bool recoilActive = false;
	unsigned int newValue = 1337;

	while (GetExitCodeProcess(procHandle, &exitStatus) && exitStatus == STILL_ACTIVE) {
		if (GetAsyncKeyState(VK_F1) & 1) { //God Mode
			godActive = !godActive;

			if (godActive) {
				WriteProcessMemory(procHandle, (PVOID)player.healthAddr, &newValue, sizeof(unsigned int), nullptr);
				std::cout << "[+] God Mode ON" << std::endl;
			}
			else {
				unsigned int normalHealth = 100;
				WriteProcessMemory(procHandle, (PVOID)player.healthAddr, &normalHealth, sizeof(unsigned int), nullptr);
				std::cout << "[-] God Mode OFF" << std::endl;
			}
		}

		if (godActive) {
			WriteProcessMemory(procHandle, (PVOID)player.healthAddr, &newValue, sizeof(unsigned int), nullptr);
			Sleep(100);
		}

		if (GetAsyncKeyState(VK_F2) & 1) { //Ammo
			ammoActive = !ammoActive;

			if (ammoActive) {
				memory::patchBytes((PVOID)"\xFF\x00", (PVOID)player.decAmmoAddr, 2, procHandle);
				WriteProcessMemory(procHandle, (PVOID)player.assaultMagazineAddr, &newValue, sizeof(unsigned int), nullptr);
				WriteProcessMemory(procHandle, (PVOID)player.pistolMagazineAddr, &newValue, sizeof(unsigned int), nullptr);
				std::cout << "[+] Increment Ammo ON" << std::endl;
			}
			else {
				unsigned int defaultMagazine = 50;
				memory::patchBytes((PVOID)"\xFF\x08", (PVOID)player.decAmmoAddr, 2, procHandle);
				WriteProcessMemory(procHandle, (PVOID)player.assaultMagazineAddr, &defaultMagazine, sizeof(unsigned int), nullptr);
				WriteProcessMemory(procHandle, (PVOID)player.pistolMagazineAddr, &defaultMagazine, sizeof(unsigned int), nullptr);
				std::cout << "[-] Increment Ammo OFF" << std::endl;
			}
		}

		if (GetAsyncKeyState(VK_F9) & 1) return 0; //Exit
	}

	std::cin.get();
}