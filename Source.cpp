#include <Windows.h>
#include <iostream>
#include <math.h>
#include <tlhelp32.h>
#include<WinUser.h>
#include"MemoryHack.h"
#include"3Daim.h"

unsigned int pbAddr[32];
unsigned int pListBase, mpBase;
unsigned int pBaseOffset = 0x0010F4F8;
unsigned int mpBaseOffset = 0x00109B74;
unsigned int HealthOffset = 0xF8;
unsigned int XposOffset = 0x34;
unsigned int YposOffset = 0x3C;
unsigned int ZposOffset = 0x38;
unsigned int AimXOffset = 0x40;
unsigned int AimYOffset = 0x44;

Entity mp;
std::vector <Entity> p;


uintptr_t procID = NULL;
uintptr_t modAddr = NULL;
HANDLE handle;
const WCHAR* GameName = L"ac_client.exe";

BOOL FindProcess(){
	procID = FindProcessIDByExeFileName(GameName);
	if (procID != NULL) {
		return true;
	}
	return false;
}

Entity ReadEntityFromGame(uintptr_t BaseAddr) {
	Entity a;
	ReadProcessMemory(handle, (BYTE*)BaseAddr + HealthOffset, &a.health, sizeof(a.health), 0);
	ReadProcessMemory(handle, (BYTE*)BaseAddr + XposOffset, &a.xpos, sizeof(a.xpos), 0);
	ReadProcessMemory(handle, (BYTE*)BaseAddr + YposOffset, &a.ypos, sizeof(a.ypos), 0);
	ReadProcessMemory(handle, (BYTE*)BaseAddr + ZposOffset, &a.zpos, sizeof(a.zpos), 0);
	ReadProcessMemory(handle, (BYTE*)BaseAddr + AimXOffset, &a.aimx, sizeof(a.aimx), 0);
	ReadProcessMemory(handle, (BYTE*)BaseAddr + AimYOffset, &a.aimy, sizeof(a.aimy), 0);
	return a;
}

void WriteEntityToGame(uintptr_t BaseAddr) {
	WriteProcessMemory(handle, (BYTE*)(BaseAddr + AimXOffset), &mp.aimx, sizeof(mp.aimx), 0);
	WriteProcessMemory(handle, (BYTE*)(BaseAddr + AimYOffset), &mp.aimy, sizeof(mp.aimy), 0);

}

Entity Aiming(Entity mainPlayer,Entity target) {
	Entity Rp;
	float dis = CalculateDistance(mainPlayer, target);
	float xdis = target.xpos - mainPlayer.xpos;
	float ydis = target.ypos - mainPlayer.ypos;
	float zdis = mainPlayer.zpos - target.zpos;
	Rp.aimy = (float)(asin((double)(ydis / dis))*180/PI);
	Rp.aimx = (float)(asin((double)(xdis / sqrt((pow(zdis, 2) + pow(xdis, 2))))) * 180 / PI);
	if (zdis < 0 )
	{
		Rp.aimx =180 - Rp.aimx;
	}
	if (Rp.aimx < 0)
	{
		Rp.aimx += 360;
	}
	
	return Rp;

}

int main() {
	BOOL toggle = FALSE;
	while (!FindProcess()) {
		std::cout << "Fail To Start Up, Auto Restart In 3 Second" << std::endl;
		Sleep(3000);
	}
	modAddr = GetMoudleBaseAddr(procID, GameName);
	handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
	system("cls");
	std::cout << "啟動成功，按下L鍵來開關自瞄" << std::endl;
	while (true)
	{
		//get main player base addr
		ReadProcessMemory(handle, (BYTE*)(modAddr + mpBaseOffset), &mpBase, sizeof(mpBase), 0);
		//get players' list base addr
		ReadProcessMemory(handle, (BYTE*)(modAddr + pBaseOffset), &pListBase, sizeof(mpBase), 0);
		//get player's base addr
		for (int i = 0; i < 32; i++) {
			ReadProcessMemory(handle, (BYTE*)(pListBase + 4 * i), &pbAddr[i], sizeof(pbAddr[0]), 0);
		}
		//put number into entities
		mp = ReadEntityFromGame(mpBase);
		for (int i = 0; i < 32; i++)
		{
			p.push_back(ReadEntityFromGame(pbAddr[i]));
		}
		//find the nearest enemy
		Entity nearest;
		nearest = FindNearestAliveEntity(mp, p);
		//calculate aiming and write to mp
		mp = Aiming(mp,nearest);
		//write mp to memory
		if (toggle == TRUE) {
			WriteEntityToGame(mpBase);
		}
		if (GetAsyncKeyState(0x4C) & 1) {
			toggle = !toggle;
		}
		p.clear();



		







	}
	
	
	
}
