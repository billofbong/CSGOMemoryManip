#pragma once
#include "MemoryManager.h"
class Process
{
public:
	Process(const char *pName, const char *mName);
	uintptr_t getBase();
	HANDLE getHandle();
	DWORD getPID();
private:
	uintptr_t base;
	HANDLE handle;
	DWORD pid;
};

