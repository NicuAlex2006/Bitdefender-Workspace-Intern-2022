// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "directories_monitor.h"
#include <fstream>
#include <chrono>
#include "clients.h"
#include <stdlib.h>     
#include <time.h>
std::ifstream fin("test.in");
std::ofstream fout("test.out");
int fileNumber[100];

int main()
{
	observer* x = new client_A;
	std::vector<std::wstring> folders;
	auto path = L"C:\\";
	for (int i = 0; i < 100; i++) {
		std::wstring tmp = path;
		tmp += L"Folder";
		tmp += std::to_wstring(i);
		tmp += L"\\";
		CreateDirectory(tmp.c_str(), NULL);
		folders.push_back( tmp );
	}
	directories_monitor* monitor = new directories_monitor(folders);
	monitor->add_observer(x);
	monitor->start();
	srand (time(NULL));
	std::vector<std::wstring> files[100];
	int test_case;
	for (test_case = 0; test_case < 100; test_case++) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		int random_folder = test_case % folders.size();
		HANDLE hFile;
		int file_number = fileNumber[random_folder] + 1;
		std::wstring path = monitor->list_of_directories[random_folder].parent_directory;
		path += L"file";
		path += std::to_wstring(file_number);
		files[random_folder].push_back(path);
		fileNumber[random_folder]++;
		hFile = CreateFile(
			path.c_str(),
			GENERIC_WRITE,
			FILE_SHARE_DELETE,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	}
	for (int i = 0; i < monitor->list_of_directories.size(); i++) {
		auto tmp = monitor->list_of_directories[i].parent_directory;
		tmp += L"*.*";
		WIN32_FIND_DATA file;
		HANDLE hFind = FindFirstFile(tmp.c_str(), &file);
		if (hFind == INVALID_HANDLE_VALUE) {
			printf("\n ERROR: FindFirstFile function failed.\n");
			ExitProcess(GetLastError());
		}
		do {
			auto path = monitor->list_of_directories[i].parent_directory;
			path += file.cFileName;
			DeleteFile(path.c_str());
		} while (FindNextFile(hFind, &file) != 0);
	}
	monitor->stop();
	delete x;
	delete monitor;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
