#include "pch.h"
#include <vector>
#include <iostream>
#include <string>
#include <filesystem>
#include "enumerate.h"

#define ERROR_BAD_ARGUMENTS -1

int enumerate(const wchar_t* folder, pfn_on_enum_files callback, void* ctx)
{
	if (!folder)
		return ERROR_BAD_ARGUMENTS;

	if (!std::filesystem::exists(folder)) {
		return ERROR_BAD_ARGUMENTS;
	}

	std::vector<std::wstring> files;
	std::vector<std::wstring> folders;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA file;
	std::wstring folder_tmp = folder;
	folder_tmp += L"*.*";

	hFind = FindFirstFile(folder_tmp.c_str(), &file);

	std::wstring tmp;
	std::wstring FinalFilePath;

	do {
		if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if ( std::wcscmp( file.cFileName, L"." ) != 0 && std::wcscmp(file.cFileName, L".." ) != 0 ) {
				tmp = folder;
				tmp += file.cFileName;
				tmp += '\\';
				folders.push_back(tmp);
			}
		}
		else {
			FinalFilePath = folder;
			FinalFilePath += file.cFileName;
			files.push_back(FinalFilePath);
		}

	} while (FindNextFile(hFind, &file));
	FindClose(hFind);

	for (const auto& file : files) {
		callback(folder, file.c_str(), ctx);
	}

	for (const auto& f : folders) {
		enumerate(f.c_str(), callback, ctx);
	}

	return 0;
}

int enumerate(const wchar_t* folder) {
	auto callback = [](const wchar_t* folder, const wchar_t* file, void* ctx) -> void {
		std::wcout << "Folder: ";
		if (folder) {
			std::wcout << folder;
		}
		std::wcout << " ;";
		std::wcout << "File: ";
		if (file) {
			std::wcout << file;
		}
		std::wcout << " ;\n";
	};
	return enumerate(folder, callback, nullptr);
}