// Readwritemem.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include<Windows.h>
#include <TlHelp32.h>
#include<string>
#include<chrono>

using namespace std;

// Uses the pointer path generated from CE to get the Player Object
DWORD findPlayer(DWORD baseAddr,HANDLE handle) {
    DWORD newAddr;
    // Pointer path the player Mana
    DWORD Offsets[] = { 0x0000001C, 0x00000004, 0x0000004C, 0x000003E0,0x000000BC };
    int playerMana;

    // Get the Addr to work with.
    ReadProcessMemory(handle, (LPCVOID)baseAddr, &newAddr, sizeof(newAddr), 0);
    wprintf(L"Addr to work with: 0x%08X\n", (DWORD)newAddr);

    for (int i = 0; i < 4; i++) {
        baseAddr = newAddr + Offsets[i];
        ReadProcessMemory(handle, (LPCVOID)baseAddr, &newAddr, sizeof(newAddr), 0);
        wprintf(L"Addr: 0x%08X\n", (DWORD)newAddr);
    }

    // Returns player Addr
    return newAddr;
}

int main()
{
    // Hook into the process memory of the window "PwnAdventure3..."
    HWND hwnd = FindWindowA(NULL, "PwnAdventure3 (32-bit, PCD3D_SM5)");
    DWORD playerAddrOffset = 0x3A8DA5D8;
    DWORD playerAddr;
    int playerMana;
    float walkSpeed = 6000000;
    float jumpSpeed = 10000;
    float jumpholdtime = 0;
    
    if (hwnd == NULL) {
        cout << "Could not find window." << endl;
    }
    else {
        DWORD procID;
        GetWindowThreadProcessId(hwnd, &procID);
        HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
        

        if (procID == NULL) {
            cout << "Could not get process ID." << endl;
        }
        else {
            // Take a snapshot of the virtual memory
            HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
            MODULEENTRY32 me32;

            hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, procID);

            me32.dwSize = sizeof(MODULEENTRY32);
            Module32First(hModuleSnap, &me32);
            
            wstring dllName = L"GameLogic.dll";
            do{
                wstring moduleName = me32.szModule;
                if (moduleName == dllName) {
                    wprintf(L"%s\n", me32.szModule);
                    wprintf(L"Base Address: 0x%08X\n", (DWORD)me32.modBaseAddr);
                    wprintf(L"Size : %i\n", me32.modBaseAddr);

                    DWORD baseAddr = (DWORD)me32.modBaseAddr + 0x00097D7C;
                    playerAddr = findPlayer(baseAddr,handle);
                }
            } while (Module32Next(hModuleSnap, &me32) == TRUE);

            CloseHandle(hModuleSnap);

            // for testing
            WriteProcessMemory(handle, (LPVOID)(playerAddr+0x00000120), &walkSpeed, sizeof(walkSpeed), 0);
            WriteProcessMemory(handle, (LPVOID)(playerAddr + 0x00000124), &jumpSpeed, sizeof(jumpholdtime), 0);
            WriteProcessMemory(handle, (LPVOID)(playerAddr + 0x00000128), &jumpholdtime, sizeof(jumpholdtime), 0);
        }
    }

    system("PAUSE");
    return 0;
}