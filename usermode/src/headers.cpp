#include "headers.hpp"
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

bool driver::attach_to_process(HANDLE driver_handle, const DWORD pid) {
	driver::Request r;
	r.process_id = reinterpret_cast<HANDLE>(pid);

	return DeviceIoControl(driver_handle, driver::codes::attach, &r, sizeof(r), &r, sizeof(r), nullptr, nullptr);
}


