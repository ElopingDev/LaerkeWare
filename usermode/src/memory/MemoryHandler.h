#pragma once
#include <Windows.h>
#include <cstdint>
#include <TlHelp32.h>
#include <vector>
#include <memory>
#include <iostream>

class MemoryHandler {
public:
    explicit MemoryHandler(HANDLE driverHandle);
    ~MemoryHandler() = default;

    bool AttachToProcess(DWORD pid);

    template <typename T>
    T ReadMemory(std::uint32_t address) {
        T temp = {};

        Request r{};
        r.process_id = reinterpret_cast<HANDLE>(m_processId);  // Use stored PID here
        r.target = reinterpret_cast<PVOID>(address);
        r.buffer = &temp;
        r.size = sizeof(T);

        DeviceIoControl(m_driverHandle, CTL_READ, &r, sizeof(r), &r, sizeof(r), nullptr, nullptr);

        return temp;
    }

    template <typename T>
    void WriteMemory(std::uint32_t address, const T& value) {
        Request r{};
        r.process_id = reinterpret_cast<HANDLE>(m_processId);  // Use stored PID here
        r.target = reinterpret_cast<PVOID>(address);
        r.buffer = const_cast<T*>(&value);
        r.size = sizeof(T);

        DeviceIoControl(m_driverHandle, CTL_WRITE, &r, sizeof(r), &r, sizeof(r), nullptr, nullptr);
    }


    template<typename T>
    inline bool readPointerArray(std::uint32_t addr, std::vector<std::shared_ptr<T>>& vec, size_t maxCount, bool /*checkPerm*/)
    {
        vec.clear();

        for (size_t i = 0; i < maxCount; i++) {
            std::uint32_t currentAddr = addr + i * sizeof(std::uint32_t);
            std::uint32_t rawPtr = ReadMemory<std::uint32_t>(currentAddr);


            if (rawPtr == 0)
                break;

            if (rawPtr < 0x19000)  // sanity check
                break;

            // Use driver read, not ReadProcessMemory:
            T obj = ReadMemory<T>(rawPtr);

            vec.emplace_back(std::make_shared<T>(obj));
        }

        return !vec.empty();
    }

    template<class T>
    inline bool readArray(std::uint32_t addr, T* array, size_t count)
    {
        for (size_t i = 0; i < count; i++) {
            array[i] = ReadMemory<T>(addr + i * sizeof(T));
        }
        return true;
    }

	void getWindowSize(int& width, int& height, int& offsetX, int& offsetY) {
		RECT rect;
        if (GetWindowRect(windowHandle, &rect)) {
            width = rect.right - rect.left;
			height = rect.bottom - rect.top;
			offsetX = rect.left;
			offsetY = rect.top;
        }
		
	}

    void getWindowHandle()
    {
		struct EnumWindowsData {
            DWORD processId;
			HWND foundWindow;
		};
        EnumWindowsData data;
        data.processId = m_processId;
        data.foundWindow = nullptr;

		EnumWindows([](HWND window, LPARAM lParam) -> BOOL 
            {
			EnumWindowsData* pData = reinterpret_cast<EnumWindowsData*>(lParam);
			DWORD windowProcessId = 0;
			GetWindowThreadProcessId(window, &windowProcessId);
			if (windowProcessId == pData->processId) 
            {
				pData->foundWindow = window;
				return FALSE; 
			}
			return TRUE; 
			},
            reinterpret_cast<LPARAM>(&data));
        if (!data.foundWindow)
            return;
        windowHandle = data.foundWindow;
			
    }


    static uint32_t getModuleBaseAddress(DWORD pid, const wchar_t* module_name) {
        uint32_t module_base = 0;
        HANDLE snap_shot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
        if (snap_shot == INVALID_HANDLE_VALUE)
            return 0;

        MODULEENTRY32W entry = {};
        entry.dwSize = sizeof(entry);

        if (Module32FirstW(snap_shot, &entry)) {
            do {
                if (_wcsicmp(entry.szModule, module_name) == 0) {
                    module_base = reinterpret_cast<uint32_t>(entry.modBaseAddr);
                    break;
                }
            } while (Module32NextW(snap_shot, &entry));
        }

        CloseHandle(snap_shot);
        return module_base;
    }

    static DWORD getProcessId(const wchar_t* process_name) {
        DWORD process_id = 0;
        HANDLE snap_shot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
        if (snap_shot == INVALID_HANDLE_VALUE)
            return 0;

        PROCESSENTRY32W entry = {};
        entry.dwSize = sizeof(entry);

        if (Process32FirstW(snap_shot, &entry)) {
            do {
                if (_wcsicmp(process_name, entry.szExeFile) == 0) {
                    process_id = entry.th32ProcessID;
                    break;
                }
            } while (Process32NextW(snap_shot, &entry));
        }

        CloseHandle(snap_shot);
        return process_id;
    }

private:
    HANDLE m_driverHandle;
    HWND windowHandle;
    DWORD m_processId = 0;
    // Control codes for DeviceIoControl
    static constexpr ULONG CTL_ATTACH = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x696, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
    static constexpr ULONG CTL_READ = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x697, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
    static constexpr ULONG CTL_WRITE = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x698, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

    // Internal struct matching driver request
    struct Request {
        HANDLE process_id;
        PVOID target;
        PVOID buffer;
        SIZE_T size;
        SIZE_T return_size;
    };
};
