#pragma once
#include "Memory.h"

namespace offset {
	uintptr_t currentWeapon = 0x364;
	uintptr_t assault = 0x354;
	uintptr_t pistol = 0x340;
	std::vector<uintptr_t> weaponAmmo = { 0x14, 0x0 };
	std::vector<uintptr_t> weaponMagazine = { 0x10, 0x0 };
	uintptr_t health = 0xEC;
	uintptr_t player = 0x17E0A8;
	uintptr_t decAmmo = 0xC73EF;
}