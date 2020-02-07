#pragma once
#include "memory_manager.h"
class process
{
public:
	process(const char *p_name, const char *m_name);
	uintptr_t getBase();
	HANDLE getHandle();
	DWORD getPID();
private:
	uintptr_t base_;
	HANDLE handle_;
	DWORD pid_;
};

