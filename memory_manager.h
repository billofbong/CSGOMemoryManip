#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>

class memory_manager
{
public:
	static DWORD find_process_id(const char * p_name);
	static uintptr_t find_module_base_address(DWORD pid, const char* m_name);
	static HANDLE find_process_handle(DWORD pid);
};

