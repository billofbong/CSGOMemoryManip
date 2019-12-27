#include "MemoryManager.h"

DWORD MemoryManager::FindProcessID(const char * pName)
{
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	Process32First(snap, &pe);
	do
	{
		if (snap != INVALID_HANDLE_VALUE)
		{
			if (!_stricmp(pe.szExeFile, pName))
				return pe.th32ProcessID;
		}
	} while (Process32Next(snap, &pe));
	CloseHandle(snap);
	return 0;
}

uintptr_t MemoryManager::FindModuleBaseAddress(DWORD pid, const char * mName)
{
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	MODULEENTRY32 me;
	me.dwSize = sizeof(MODULEENTRY32);
	Module32First(snap, &me);
	do
	{
		if (snap != INVALID_HANDLE_VALUE)
		{
			if (!_stricmp(me.szModule, mName))
				return (uintptr_t) me.modBaseAddr;
		}
	} while (Module32Next(snap, &me));
	return 0;
}

HANDLE MemoryManager::FindProcessHandle(DWORD pid)
{
	return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
}
