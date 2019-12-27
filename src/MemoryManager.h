#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>

class MemoryManager
{
public:
	DWORD FindProcessID(const char * pName);
	uintptr_t FindModuleBaseAddress(DWORD pid, const char* mName);
	HANDLE FindProcessHandle(DWORD pid);
};

