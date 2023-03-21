#include "directories_monitor.h"

directories_monitor::directories_monitor(const std::vector<std::wstring>& path_to_watch) {
	/*for (auto const& file : std::filesystem::directory_iterator(path_to_watch)) {
		paths[file.path().string()] = std::filesystem::last_write_time(file);
	}*/
	int index = 0;
	dwChangeHandles = new HANDLE[2 * path_to_watch.size()];
	for (const auto& path : path_to_watch) {
		dwChangeHandles[index * 2] = FindFirstChangeNotification(path.c_str(), FALSE, FILE_NOTIFY_CHANGE_FILE_NAME);
		if (dwChangeHandles[index * 2] == INVALID_HANDLE_VALUE) {
			printf("\n ERROR: FindFirstChangeNotification function failed.\n");
			ExitProcess(GetLastError());
		}
		dwChangeHandles[index * 2 + 1] = FindFirstChangeNotification(path.c_str(), FALSE, FILE_NOTIFY_CHANGE_DIR_NAME);
		if (dwChangeHandles[index * 2 + 1] == INVALID_HANDLE_VALUE) {
			printf("\n ERROR: FindFirstChangeNotification function failed.\n");
			ExitProcess(GetLastError());
		}
		auto tmp = path;
		tmp += L"*.*";
		WIN32_FIND_DATA file;
		HANDLE hFind = FindFirstFile(tmp.c_str(), &file);
		list_of_directories.push_back({});
		list_of_directories[index].parent_directory = path;
		do {
			if (std::wcscmp(file.cFileName, L".") != 0 && std::wcscmp(file.cFileName, L"..") != 0) {
				list_of_directories[index].paths[file.cFileName] = file;
			}
		} while (FindNextFile(hFind, &file) != 0);
		index++;
	}
}

void directories_monitor::add_observer(observer* obs) {
	observers_list.push_back(obs);
}

bool directories_monitor::contains(const std::wstring& key, int index) {
	return (list_of_directories[index].paths.find(key) != list_of_directories[index].paths.end());
}

void directories_monitor::send_to_observer(std::wstring file_path, int type, std::wstring& directory_path) {
	for (const auto& p : observers_list) {
		p->on_directory_changed(file_path, type, directory_path);
	}
}

void directories_monitor::check_for_erase(int index) {
	for (auto it = list_of_directories[index].paths.begin(); it != list_of_directories[index].paths.end(); ) {
		auto file = it->second;
		auto tmp = list_of_directories[index].parent_directory;
		if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			tmp += file.cFileName;
			tmp += '\\';
		}
		else {
			tmp += file.cFileName;
		}
		if (GetFileAttributes(tmp.c_str()) == INVALID_FILE_ATTRIBUTES) {
			if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				send_to_observer(it->first, 5, list_of_directories[index].parent_directory);
				it = list_of_directories[index].paths.erase(it);
			}
			else {
				send_to_observer(it->first, 4, list_of_directories[index].parent_directory);
				it = list_of_directories[index].paths.erase(it);
			}
		}
		else
			++it;
	}
}

void directories_monitor::check_for_modifing_and_adding(int index) {
	auto tmp_path = list_of_directories[index].parent_directory;
	tmp_path += L"*.*";
	WIN32_FIND_DATA file;
	HANDLE hFind = FindFirstFile(tmp_path.c_str(), &file);
	do {
		if (std::wcscmp(file.cFileName, L".") != 0 && std::wcscmp(file.cFileName, L"..") != 0) {
			auto tmp = list_of_directories[index].parent_directory;
			if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				tmp += file.cFileName;
				tmp += '\\';
			}
			else {
				tmp += file.cFileName;
			}
			if (!contains(file.cFileName, index)) {
				if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					send_to_observer(file.cFileName, 1, list_of_directories[index].parent_directory);
				}
				else {
					send_to_observer(file.cFileName, 0, list_of_directories[index].parent_directory);
				}
				list_of_directories[index].paths[file.cFileName] = file;
			}
			else {
				if (CompareFileTime(&list_of_directories[index].paths[file.cFileName].ftLastWriteTime, &file.ftLastWriteTime)) {
					if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
						send_to_observer(file.cFileName, 3, list_of_directories[index].parent_directory);
					}
					else {
						send_to_observer(file.cFileName, 2, list_of_directories[index].parent_directory);
					}
					list_of_directories[index].paths[file.cFileName] = file;
				}
			}
		}
	} while (FindNextFile(hFind, &file) != 0);
}

void directories_monitor::start() {
	while (1) {
		DWORD dwWaitStatus = WaitForMultipleObjects(2 * list_of_directories.size(), dwChangeHandles,
			FALSE, INFINITE);
		if (dwWaitStatus != WAIT_TIMEOUT) {
			//check for erase
			int index = dwWaitStatus;
			index /= 2;
			check_for_erase(index);

			///check for modifing or adding
			check_for_modifing_and_adding(index);
			FindNextChangeNotification(dwChangeHandles[dwWaitStatus]);
		}
		else {
			std::cout << "No changes made in the timeout period!\n";
		}
	}
}

directories_monitor::~directories_monitor() {
	delete[] dwChangeHandles;
}