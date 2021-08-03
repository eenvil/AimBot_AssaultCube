#pragma once
#include <Windows.h>
#include <iostream>
#include <math.h>
#include <tlhelp32.h>
#include<vector>
uintptr_t FindProcessIDByExeFileName(const wchar_t* szExeFile) {
	uintptr_t procID = 0;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapShotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(snapShotHandle, &entry))
	{
		while (Process32Next(snapShotHandle, &entry))
		{
			if (wcscmp(entry.szExeFile, szExeFile) == 0)
			{
				procID = (uintptr_t)entry.th32ProcessID;
				break;
			}
		}
	}
	CloseHandle(snapShotHandle);
	return procID;
	
}
uintptr_t GetMoudleBaseAddr(DWORD procID, const wchar_t* modName) {
	uintptr_t modBaseAddr = 0;
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procID);
	if (hsnap != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hsnap, &modEntry)) {
			do {
				if (wcscmp(modEntry.szModule, modName) == 0) {
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hsnap,&modEntry));
		}
	}
	CloseHandle(hsnap);
	return modBaseAddr;
}
uintptr_t FindDMAAddr(HANDLE h, uintptr_t ptr, std::vector<unsigned int> offsets) {
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); i++)
	{
		ReadProcessMemory(h, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];
	}
	return addr;
}
