#include "Process.h"
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
	bool renderWhenOccluded;
	bool renderWhenUnoccluded;
	bool fullBloom;
	unsigned char junk2[14];
};

void trigger(Process p, uintptr_t localPlayer);
void bhop(Process p, uintptr_t localPlayer);
void glow(Process p, uintptr_t localPlayer, bool enabled);
std::string center(std::string toCenter, int width)
{
	int length = toCenter.length();
	int spacing = (width - length) / 2;
	return std::string(spacing, ' ') + toCenter + std::string(spacing, ' ');
}

int main()
{
	Process p("csgo.exe", "client_panorama.dll");
	while (p.getBase() == 0)
	{
		std::cout << "CSGO not detected! Launch CSGO then ";
		system("pause");
		p = Process("csgo.exe", "client_panorama.dll");
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
	uintptr_t localPlayer;
	bool glowEnabled = true;
	bool glowKeyPressed = false;
	while (true)
	{
		ReadProcessMemory(p.getHandle(), LPCVOID(p.getBase() + signatures::dwLocalPlayer), &localPlayer, sizeof(uintptr_t), NULL);
		trigger(p, localPlayer);
		bhop(p, localPlayer);
		if (GetAsyncKeyState(VK_F1) && !glowKeyPressed)
		{
			glowEnabled = !glowEnabled;
			glowKeyPressed = true;
		}
		if (!GetAsyncKeyState(VK_F1) && glowKeyPressed)
			glowKeyPressed = false;
		glow(p, localPlayer, glowEnabled);
		Sleep(1);
	}
	return 0;
}

void trigger(Process p, uintptr_t localPlayer)
{
	int ibuffer;
	int crosshairID;
	int teamNum;
	uintptr_t entity;
	ReadProcessMemory(p.getHandle(), LPCVOID(localPlayer + netvars::m_iTeamNum), &teamNum, sizeof(int), NULL);
	ReadProcessMemory(p.getHandle(), LPCVOID(localPlayer + netvars::m_iCrosshairId), &crosshairID, sizeof(int), NULL);
	ReadProcessMemory(p.getHandle(), LPCVOID(p.getBase() + signatures::dwEntityList + ((crosshairID - 1) * 0x10)), &entity, sizeof(uintptr_t), NULL);
	int eTeamNum;
	ReadProcessMemory(p.getHandle(), LPCVOID(entity + netvars::m_iTeamNum), &eTeamNum, sizeof(int), NULL);
	if (crosshairID >= 0 && crosshairID <= 64 && eTeamNum + teamNum == 5 && GetAsyncKeyState(VK_XBUTTON1))
	{
		Sleep(10);
		ibuffer = 6;
		WriteProcessMemory(p.getHandle(), LPVOID(p.getBase() + signatures::dwForceAttack), &ibuffer, sizeof(int), NULL);
		Sleep(50);
		ibuffer = 4;
		WriteProcessMemory(p.getHandle(), LPVOID(p.getBase() + signatures::dwForceAttack), &ibuffer, sizeof(int), NULL);
	}

}
void bhop(Process p, uintptr_t localPlayer)
{
	int ibuffer;
	BYTE flags;
	ReadProcessMemory(p.getHandle(), LPCVOID(localPlayer + netvars::m_fFlags), &flags, sizeof(flags), NULL);
	if (flags & 1 << 0 && GetAsyncKeyState(VK_SPACE))
	{
		ibuffer = 6;
		WriteProcessMemory(p.getHandle(), LPVOID(p.getBase() + signatures::dwForceJump), &ibuffer, sizeof(int), NULL);
		Sleep(50);
		ibuffer = 4;
		WriteProcessMemory(p.getHandle(), LPVOID(p.getBase() + signatures::dwForceJump), &ibuffer, sizeof(int), NULL);
	}
}
void glow(Process p, uintptr_t localPlayer, bool enabled)
{
	uintptr_t glowManager;
	uintptr_t glowIndex;
	int teamNum;
	ReadProcessMemory(p.getHandle(), LPCVOID(localPlayer + netvars::m_iTeamNum), &teamNum, sizeof(int), NULL);
	ReadProcessMemory(p.getHandle(), LPCVOID(p.getBase() + signatures::dwGlowObjectManager), &glowManager, sizeof(uintptr_t), NULL);
	for (int i = 0; i <= 64; i++)
	{
		uintptr_t entity = 0;
		int eTeamNum;
		bool isDormant;
		ReadProcessMemory(p.getHandle(), LPCVOID(p.getBase() + signatures::dwEntityList + (i * 0x10)), &entity, sizeof(uintptr_t), NULL);
		ReadProcessMemory(p.getHandle(), LPCVOID(entity + signatures::m_bDormant), &isDormant, sizeof(bool), NULL);
		if (isDormant)
			continue;
		ReadProcessMemory(p.getHandle(), LPCVOID(entity + netvars::m_iTeamNum), &eTeamNum, sizeof(int), NULL);
		if (teamNum == eTeamNum)
			continue;
		ReadProcessMemory(p.getHandle(), LPCVOID(entity + netvars::m_iGlowIndex), &glowIndex, sizeof(uintptr_t), NULL);
		glowObject glow;
		ReadProcessMemory(p.getHandle(), LPCVOID(glowManager + (0x38 * glowIndex) + 0x04), &glow, sizeof(glowObject), NULL);
		glow.r = 1.0f;
		glow.g = 0.0f;
		glow.b = 0.0f;
		glow.a = enabled ? 1.0f : 0.0f;
		glow.bloom = .75f;
		glow.renderWhenOccluded = false;
		glow.renderWhenUnoccluded = true;
		glow.fullBloom = false;
		WriteProcessMemory(p.getHandle(), LPVOID(glowManager + (0x38 * glowIndex) + 0x04), &glow, sizeof(glowObject), NULL);
	}
}