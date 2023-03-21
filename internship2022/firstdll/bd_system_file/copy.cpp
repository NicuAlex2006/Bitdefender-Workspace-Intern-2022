#include "pch.h"
#include "copy.h"
#define ERROR_BAD_ARGUMENTS -1
#define ERROR_FILE_EXIST -2
#include <vector>
#include <iostream>
#include <string>
#include <filesystem>

void print(const wchar_t* s) {
	int size = wcslen(s);
	for (int i = 0; i < size; i++) {
		std::cout << (char) * (s + i);
	}
}

void copyFile_(const wchar_t* inDir, const wchar_t* outDir) {
	std::cout << "In: "; 
	print(inDir);
	std::cout << "\n";
	std::cout << "Out: ";
	print(outDir);
	std::cout << "\n";
	CopyFile(inDir, outDir, 1);
	DWORD Error = GetLastError();
	std::cerr << Error << std::endl;
}
int copy_directory(const wchar_t* folder_to_copy, const wchar_t* folder_destination) {
	if (!folder_to_copy)
		return ERROR_BAD_ARGUMENTS;
	if (!std::filesystem::exists(folder_to_copy)) {
		return ERROR_BAD_ARGUMENTS;
	}
	if (!folder_destination)
		return ERROR_BAD_ARGUMENTS;
	if (!std::filesystem::exists(folder_destination)) {
		return ERROR_BAD_ARGUMENTS;
	}
	std::vector<std::wstring> files;
	std::vector<std::wstring> folders;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA file;
	std::wstring folder_tmp = folder_to_copy;
	folder_tmp += L"*.*";

	hFind = FindFirstFile(folder_tmp.c_str(), &file);

	std::wstring outPath;
	std::wstring inPath;

	do {
		if (std::wcscmp(file.cFileName, L".") != 0 && std::wcscmp(file.cFileName, L"..") != 0) {
			inPath = folder_to_copy;
			inPath += file.cFileName;		
			if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				inPath += '\\';
				outPath = folder_destination;
				outPath += file.cFileName;
				outPath += '\\';
				CreateDirectory(outPath.c_str(), NULL);
				copy_directory(inPath.c_str(), outPath.c_str());
			}
			else {
				outPath = folder_destination;
				outPath += file.cFileName;
				copyFile_(inPath.c_str(), outPath.c_str());
			}
		}

	} while (FindNextFile(hFind, &file));
	FindClose(hFind);
	return 0;
}