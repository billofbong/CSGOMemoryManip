#include "process.h"



process::process(const char *p_name, const char *m_name)
{
	memory_manager mm;
	process::pid_ = mm.find_process_id(p_name);
	process::base_ = mm.find_module_base_address(mm.find_process_id(p_name), m_name);
	process::handle_ = mm.find_process_handle(process::pid_);
}

uintptr_t process::getBase()
{
	return process::base_;
}

HANDLE process::getHandle()
{
	return process::handle_;
}

DWORD process::getPID()
{
	return process::pid_;
}
