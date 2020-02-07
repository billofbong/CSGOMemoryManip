#include "process.h"
#include "offsets.h"
#include <iostream>
#include <string>

struct glowObject
{
	float r;
	float g;
	float b;
	float a;
	unsigned char junk0[8];
	float bloom;
	unsigned char junk1[4];
	bool render_when_occluded;
	bool render_when_unoccluded;
	bool full_bloom;
	unsigned char junk2[14];
};

void trigger(process p, uintptr_t local_player);
void bhop(process p, uintptr_t local_player);
void glow(process p, uintptr_t local_player, bool enabled);
std::string center(const std::string& to_center, const int width)
{
	const int length = to_center.length();
	const int spacing = (width - length) / 2;
	return std::string(spacing, ' ') + to_center + std::string(spacing, ' ');
}

int main()
{
	process p("csgo.exe", "client_panorama.dll");
	while (p.getBase() == 0)
	{
		std::cout << "CSGO not detected! Launch CSGO then ";
		system("pause");
		p = process("csgo.exe", "client_panorama.dll");
	}
	std::cout << std::string(80, '=') << std::endl;
	std::cout << center("billofbong's epic cheat hack", 80) << std::endl;
	std::cout << std::string(80, '=') << std::endl;
	std::cout << "csgo.exe PID: " << std::dec << p.getPID() << std::endl;
	std::cout << "client_panorama.dll base: 0x" << std::hex << p.getBase() << std::endl;
	std::cout << std::string(80, '=') << std::endl;
	std::cout << "Hotkeys:" << std::endl;
	std::cout << "F1:     Toggle Glow" << std::endl;
	std::cout << "Mouse4: Triggerbot" << std::endl;
	std::cout << "Space:  Bunnyhop" << std::endl;
	uintptr_t local_player;
	bool glowEnabled = true;
	bool glowKeyPressed = false;
	while (true)
	{
		ReadProcessMemory(p.getHandle(), LPCVOID(p.getBase() + signatures::dwLocalPlayer), &local_player, sizeof(uintptr_t), NULL);
		trigger(p, local_player);
		bhop(p, local_player);
		if (GetAsyncKeyState(VK_F1) && !glowKeyPressed)
		{
			glowEnabled = !glowEnabled;
			glowKeyPressed = true;
		}
		if (!GetAsyncKeyState(VK_F1) && glowKeyPressed)
			glowKeyPressed = false;
		glow(p, local_player, glowEnabled);
		Sleep(1);
	}
}

void trigger(process p, const uintptr_t local_player)
{
	int i_buffer;
	int crosshair_id;
	int team_num;
	uintptr_t entity;
	ReadProcessMemory(p.getHandle(), LPCVOID(local_player + netvars::m_iTeamNum), &team_num, sizeof(int), NULL);
	ReadProcessMemory(p.getHandle(), LPCVOID(local_player + netvars::m_iCrosshairId), &crosshair_id, sizeof(int), NULL);
	ReadProcessMemory(p.getHandle(), LPCVOID(p.getBase() + signatures::dwEntityList + ((crosshair_id - 1) * 0x10)), &entity, sizeof(uintptr_t), NULL);
	int e_team_num;
	ReadProcessMemory(p.getHandle(), LPCVOID(entity + netvars::m_iTeamNum), &e_team_num, sizeof(int), NULL);
	if (crosshair_id >= 0 && crosshair_id <= 64 && e_team_num + team_num == 5 && GetAsyncKeyState(VK_XBUTTON1))
	{
		Sleep(10);
		i_buffer = 6;
		WriteProcessMemory(p.getHandle(), LPVOID(p.getBase() + signatures::dwForceAttack), &i_buffer, sizeof(int), NULL);
		Sleep(50);
		i_buffer = 4;
		WriteProcessMemory(p.getHandle(), LPVOID(p.getBase() + signatures::dwForceAttack), &i_buffer, sizeof(int), NULL);
	}

}
void bhop(process p, const uintptr_t local_player)
{
	int i_buffer;
	BYTE flags;
	ReadProcessMemory(p.getHandle(), LPCVOID(local_player + netvars::m_fFlags), &flags, sizeof(flags), NULL);
	if (flags & 1 << 0 && GetAsyncKeyState(VK_SPACE))
	{
		i_buffer = 6;
		WriteProcessMemory(p.getHandle(), LPVOID(p.getBase() + signatures::dwForceJump), &i_buffer, sizeof(int), NULL);
		Sleep(50);
		i_buffer = 4;
		WriteProcessMemory(p.getHandle(), LPVOID(p.getBase() + signatures::dwForceJump), &i_buffer, sizeof(int), NULL);
	}
}
void glow(process p, const uintptr_t local_player, const bool enabled)
{
	uintptr_t glowManager;
	uintptr_t glowIndex;
	int team_num;
	ReadProcessMemory(p.getHandle(), LPCVOID(local_player + netvars::m_iTeamNum), &team_num, sizeof(int), NULL);
	ReadProcessMemory(p.getHandle(), LPCVOID(p.getBase() + signatures::dwGlowObjectManager), &glowManager, sizeof(uintptr_t), NULL);
	for (int i = 0; i <= 64; i++)
	{
		uintptr_t entity = 0;
		int e_team_num;
		bool is_dormant;
		ReadProcessMemory(p.getHandle(), LPCVOID(p.getBase() + signatures::dwEntityList + (i * 0x10)), &entity, sizeof(uintptr_t), NULL);
		ReadProcessMemory(p.getHandle(), LPCVOID(entity + signatures::m_bDormant), &is_dormant, sizeof(bool), NULL);
		if (is_dormant)
			continue;
		ReadProcessMemory(p.getHandle(), LPCVOID(entity + netvars::m_iTeamNum), &e_team_num, sizeof(int), NULL);
		if (team_num == e_team_num)
			continue;
		ReadProcessMemory(p.getHandle(), LPCVOID(entity + netvars::m_iGlowIndex), &glowIndex, sizeof(uintptr_t), NULL);
		glowObject glow;
		ReadProcessMemory(p.getHandle(), LPCVOID(glowManager + (0x38 * glowIndex) + 0x04), &glow, sizeof(glowObject), NULL);
		glow.r = 1.0f;
		glow.g = 0.0f;
		glow.b = 0.0f;
		glow.a = enabled ? 1.0f : 0.0f;
		glow.bloom = .75f;
		glow.render_when_occluded = false;
		glow.render_when_unoccluded = true;
		glow.full_bloom = false;
		WriteProcessMemory(p.getHandle(), LPVOID(glowManager + (0x38 * glowIndex) + 0x04), &glow, sizeof(glowObject), NULL);
	}
}