#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <random>
#include <string>
#include <windows.h>
#include <winternl.h>
#include <iostream>
#define code_eac CTL_CODE(FILE_DEVICE_UNKNOWN, 0x900, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

HANDLE driver_handle;

typedef struct _ba {
	INT32 security;
	INT32 process_id;
	ULONGLONG* address;
} ba, * pba;

bool find_driver() {
	// implement your own method

	return true;
}

void call_eac_hook() {

	uintptr_t image_address = { NULL };
	_ba arguments = { NULL };


	DeviceIoControl(driver_handle, code_eac, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
}

int main()
{
	find_driver();
	call_eac_hook();
}
