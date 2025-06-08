#include "MemoryHandler.h"

MemoryHandler::MemoryHandler(HANDLE driverHandle)
    : m_driverHandle(driverHandle) {
}

bool MemoryHandler::AttachToProcess(DWORD pid) {
    m_processId = pid;
    Request r{};
    r.process_id = reinterpret_cast<HANDLE>(pid);
    getWindowHandle();

    return DeviceIoControl(m_driverHandle, CTL_ATTACH, &r, sizeof(r), &r, sizeof(r), nullptr, nullptr);
}
