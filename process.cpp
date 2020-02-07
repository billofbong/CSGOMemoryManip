#include "process.h"



process::process(const char *p_name, const char *m_name)
{
	process::pid_ = memory_manager::find_process_id(p_name);
	process::base_ = memory_manager::find_module_base_address(memory_manager::find_process_id(p_name), m_name);
	process::handle_ = memory_manager::find_process_handle(process::pid_);
}

uintptr_t process::getBase() const
{
	return process::base_;
}

HANDLE process::getHandle() const
{
	return process::handle_;
}

DWORD process::getPID() const
{
	return process::pid_;
}
