#include "Process.h"



Process::Process(const char *pName, const char *mName)
{
	MemoryManager mm;
	Process::pid = mm.FindProcessID(pName);
	Process::base = mm.FindModuleBaseAddress(mm.FindProcessID(pName), mName);
	Process::handle = mm.FindProcessHandle(Process::pid);
}

uintptr_t Process::getBase()
{
	return Process::base;
}

HANDLE Process::getHandle()
{
	return Process::handle;
}

DWORD Process::getPID()
{
	return Process::pid;
}
