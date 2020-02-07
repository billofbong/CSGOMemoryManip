#include "memory_manager.h"

DWORD memory_manager::find_process_id(const char * p_name)
{
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	Process32First(snap, &pe);
	do
	{
		if (snap != INVALID_HANDLE_VALUE)
		{
			if (!_stricmp(pe.szExeFile, p_name))
				return pe.th32ProcessID;
		}
	} while (Process32Next(snap, &pe));
	CloseHandle(snap);
	return 0;
}

uintptr_t memory_manager::find_module_base_address(const DWORD pid, const char * m_name)
{
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	MODULEENTRY32 me;
	me.dwSize = sizeof(MODULEENTRY32);
	Module32First(snap, &me);
	do
	{
		if (snap != INVALID_HANDLE_VALUE)
		{
			if (!_stricmp(me.szModule, m_name))
				return uintptr_t(me.modBaseAddr);
		}
	} while (Module32Next(snap, &me));
	return 0;
}

HANDLE memory_manager::find_process_handle(const DWORD pid)
{
	return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
}
