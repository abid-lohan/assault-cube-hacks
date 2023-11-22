import pymem
import win32api
import time

PLAYER = 0x17E0A8
DEC_AMMO = 0xC73EF

HP_OFFSET = 0xEC
ASSAULT_OFFSET = 0x354
PISTOL_OFFSET = 0x340
AMMO_OFFSET = 0x14
MAGAZINE_OFFSET = 0x10

def hack():
	print("Welcome to Sphere!\nAn Assault Cube External Cheat made by ruhptura.\n")

	p_handle = pymem.Pymem('ac_client.exe')
	for module in list(p_handle.list_modules()):
		if module.name == 'ac_client.exe':
			client = module.lpBaseOfDll

	if not client:
		print("Module not found. Exiting...")
		return

	local_player = p_handle.read_uint(client + PLAYER)
	local_player_hp = p_handle.read_uint(local_player + HP_OFFSET)

	if not local_player:
		print("Player not found. Exiting...")
		return

	print("Player found!")
	print("Health:", local_player_hp)
	print("--------------------------------------------")
	print("[i] F1: God Mode ON/OFF")
	print("[i] F2: Increment Ammo Mode ON/OFF")
	print("[i] F9: Close Sphere")
	print("--------------------------------------------")

	god_mode = False
	inc_ammo = False

	# Hack Loop

	while(True):
		time.sleep(0.01)

		if win32api.GetAsyncKeyState(0x78):
			print("Exiting...")
			return

		# Actions

		if win32api.GetAsyncKeyState(0x70) & 1: #God Mode
			god_mode = not god_mode

			if not god_mode:
				print("[-] God Mode OFF")
				p_handle.write_uint(local_player + HP_OFFSET, 100)
			else:
				print("[+] God Mode ON")

		if god_mode:
			p_handle.write_uint(local_player + HP_OFFSET, 1337)

		if win32api.GetAsyncKeyState(0x71) & 1: #Ammo
			inc_ammo = not inc_ammo

			assault = p_handle.read_uint(local_player + ASSAULT_OFFSET)
			pistol = p_handle.read_uint(local_player + PISTOL_OFFSET)

			if inc_ammo:
				print("[+] Increment Ammo ON")
				p_handle.write_bytes(client + DEC_AMMO, b"\xff\x00", 2)
				
				p_handle.write_uint(p_handle.read_uint(assault + MAGAZINE_OFFSET), 1337)
				p_handle.write_uint(p_handle.read_uint(pistol + MAGAZINE_OFFSET), 1337)

			if not inc_ammo:
				print("[-] Increment Ammo OFF")
				p_handle.write_bytes(client + DEC_AMMO, b"\xff\x08", 2)

				p_handle.write_uint(p_handle.read_uint(assault + MAGAZINE_OFFSET), 40)
				p_handle.write_uint(p_handle.read_uint(pistol + MAGAZINE_OFFSET), 50)


if __name__ == '__main__':
	hack()