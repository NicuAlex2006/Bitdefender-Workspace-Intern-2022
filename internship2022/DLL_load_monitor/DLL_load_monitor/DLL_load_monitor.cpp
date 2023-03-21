#define MyFunctions _declspec(dllexport)
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <thread>
#include <Windows.h>
#include <map>
#include <psapi.h>
#include "nlohmann/json.hpp"

struct CPU_Stats {
    ULARGE_INTEGER CPU, SysCPU, UserCPU;
};

static int numProcessors;
std::map<HANDLE, CPU_Stats> CPU_data;

extern "C" {
    MyFunctions void init_CPU(HANDLE& hProcess) {
        ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
        SYSTEM_INFO sysInfo;
        FILETIME ftime, fsys, fuser;

        GetSystemInfo(&sysInfo);
        numProcessors = sysInfo.dwNumberOfProcessors;

        GetSystemTimeAsFileTime(&ftime);
        memcpy(&lastCPU, &ftime, sizeof(FILETIME));

        GetProcessTimes(hProcess, &ftime, &ftime, &fsys, &fuser);
        memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
        memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
        CPU_data[hProcess].CPU = lastCPU;
        CPU_data[hProcess].SysCPU = lastSysCPU;
        CPU_data[hProcess].UserCPU = lastUserCPU;
    }

    MyFunctions double getCurrentValue_CPU(HANDLE hProcess) {
        FILETIME ftime, fsys, fuser;
        ULARGE_INTEGER now, sys, user;
        double percent;

        GetSystemTimeAsFileTime(&ftime);
        memcpy(&now, &ftime, sizeof(FILETIME));

        GetProcessTimes(hProcess, &ftime, &ftime, &fsys, &fuser);
        memcpy(&sys, &fsys, sizeof(FILETIME));
        memcpy(&user, &fuser, sizeof(FILETIME));
        ULARGE_INTEGER lastSysCPU = CPU_data[hProcess].SysCPU;
        ULARGE_INTEGER lastUserCPU = CPU_data[hProcess].UserCPU;
        ULARGE_INTEGER lastCPU = CPU_data[hProcess].CPU;
        percent = (sys.QuadPart - lastSysCPU.QuadPart) +
            (user.QuadPart - lastUserCPU.QuadPart);
        if (now.QuadPart == lastCPU.QuadPart)
            return 0;
        percent /= (now.QuadPart - lastCPU.QuadPart);
        percent /= numProcessors;
        lastCPU = now;
        lastUserCPU = user;
        lastSysCPU = sys;

        return percent * 100;
    }

    struct ProcessInfo {
        std::string ProcessName;
        std::string hMods[1024];
        int PID, virtualMem, physMem;
        double CPU;
    };

    MyFunctions int PrintProcessInfo(DWORD pID, nlohmann::json& j, int &index) {
        TCHAR ProcessName[MAX_PATH];
        HMODULE hMods[1024];
        std::wstring s;

        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
            PROCESS_VM_READ,
            FALSE, pID);

        setlocale(LC_ALL, "");
        if (NULL != hProcess) {
            DWORD cbNeeded;

            if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
                GetModuleBaseName(hProcess, hMods[0], ProcessName, sizeof(ProcessName) / sizeof(TCHAR));
                std::string x = ProcessName;
                j[index]["ProcessName"] = x;
                j[index]["PID"] = pID;
                printf("%ls  (PID: %u)\n", ProcessName, pID);

                PROCESS_MEMORY_COUNTERS_EX pmc;
                GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
                SIZE_T virtualMemUsedbyMe = pmc.PrivateUsage;

                SIZE_T physMemUsedByMe = pmc.WorkingSetSize;

                j[index]["virtualMemUsed"] = virtualMemUsedbyMe;
                j[index]["physMemUsed"] = physMemUsedByMe;
                printf("Virtual Memory Used: %d bytes\n", virtualMemUsedbyMe);
                printf("Physical Memory Used(RAM): %d bytes\n", physMemUsedByMe);

                double procent_CPU = getCurrentValue_CPU(hProcess);

                j[index]["CPU"] = procent_CPU;
                printf("CPU percentage use: %lf%\n", procent_CPU);

                int ModIndex = 0;
                for (int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
                    TCHAR ModName[MAX_PATH];
                    if (GetModuleFileNameEx(hProcess, hMods[i], ModName, sizeof(ModName) / sizeof(TCHAR))) {
                        std::string x = ModName;
                        j[index]["Modules"][ModIndex]["ModuleName"] = x;
                        printf("\t%ls (0x%08X)\n", ModName, hMods[i]);
                        ModIndex++;
                    }
                }
                index++;
            }
            else {
                std::cout << "Can not access the Process( PID: " << pID << " )\n";
            }
        }
        else {
            std::cout << "Can not access the Process( PID: " << pID << " )\n";
        }
        CloseHandle(hProcess);
        return index;
    }

    MyFunctions int SolveProcesses() {
        std::ofstream o_json("C:\\Users\\anicu\\git\\bdinternship\\internship2022\\DLL_load_monitor\\DLL_load_monitor\\load_monitor.json");
        DWORD Processes[2048], cbNeeded, cProcesses;
        if (!EnumProcesses(Processes, sizeof(Processes), &cbNeeded)) {
            return 1;
        }
        cProcesses = cbNeeded / sizeof(DWORD);
        for (int i = 0; i < cProcesses; i++) {
            if (Processes[i] != 0) {
                HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
                    PROCESS_VM_READ,
                    FALSE, Processes[i]);
                init_CPU(hProcess);
            }
        }
        int index = 0;
        nlohmann::json j;
        for (int i = 0; i < cProcesses; i++) {
            if (Processes[i] != 0) {
                index = PrintProcessInfo(Processes[i], j, index);
            }
        }
        o_json << std::setw(4) << j;
        o_json.close();
        return 0;
    }
}