#include "directories_monitor.h"

int WaitForObjects::WaitForMultipleObjects(DWORD size, const HANDLE* array) {
	int st = 0;
	int dr = min(size - 1, 63);
	while (1) {
		while (dr < size - 1) {
			HANDLE* tmp_array = new HANDLE[dr - st + 1];
			for (int i = st; i <= dr; i++) {
				tmp_array[i - st] = array[i];
			}
			DWORD ans = ::WaitForMultipleObjects(dr - st + 1, tmp_array, FALSE, 30);
			if (ans != WAIT_TIMEOUT) {
				return st + ans;
			}
			st = dr + 1;
			dr = min(size - 1, st + 63);
			delete[] tmp_array;
		}
		HANDLE* tmp_array = new HANDLE[dr - st + 1];
		for (int i = st; i <= dr; i++) {
			tmp_array[i - st] = array[i];
		}
		DWORD ans = ::WaitForMultipleObjects(dr - st + 1, tmp_array, FALSE, 30);
		if (ans != WAIT_TIMEOUT) {
			return st + ans;
		}
		delete[] tmp_array;
	}
}

directories_monitor::directories_monitor(const std::vector<std::wstring>& path_to_watch) {
	int index = 0;
	dwChangeHandles = new HANDLE[path_to_watch.size()];
	for (const auto& path : path_to_watch) {
		dwChangeHandles[index] = FindFirstChangeNotification(path.c_str(), FALSE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME );
		if (dwChangeHandles[index] == INVALID_HANDLE_VALUE) {
			printf("\n ERROR: FindFirstChangeNotification function failed.\n");
			ExitProcess(GetLastError());
		}
		auto tmp = path;
		tmp += L"*.*";
		WIN32_FIND_DATA file;
		HANDLE hFind = FindFirstFile(tmp.c_str(), &file);
		if (hFind == INVALID_HANDLE_VALUE) {
			printf("\n ERROR: FindFirstFile function failed.\n");
			ExitProcess(GetLastError());
		}
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

void directories_monitor::stop() {
	while (!GetAsyncKeyState(VK_ESCAPE)) {}
	SetEvent(EscapeEvent);
	if (t_notification.joinable()) {
		t_notification.join();
	}
	thread_pool.Stop();
}

void directories_monitor::add_observer(observer* obs) {
	std::unique_lock<std::mutex> lock(mutex);
	observers_list.push_back(obs);
	lock.unlock();
}

bool directories_monitor::contains(const std::wstring& key, int index) {
	try {
		return (list_of_directories[index].paths.find(key) != list_of_directories[index].paths.end());
	}
	catch (...) {
		printf("\n ERROR: Contains function failed.\n");
		ExitProcess(GetLastError());
	}
}

void send_to_observer_TP(void* p) {
	auto x = static_cast<params*> (p);
	directories_monitor* obj = x->obj;
	pfn_send_to_observer pfn = (x->pfn);
	auto file_path = x->file_path, directory_path = x->directory_path;
	int type = x->type;
	(obj->*pfn)(file_path, type, directory_path);
	delete x;
}

void directories_monitor::send_to_observer(std::wstring file_path, int type, std::wstring directory_path) {
	std::unique_lock<std::mutex> lock(mutex);
	for (const auto& p : observers_list) {
		p->on_directory_changed(file_path, type, directory_path);
	}
	lock.unlock();
	return;
}

void* directories_monitor::build_params(std::wstring file_path, int type, std::wstring directory_path) {
	params* x = new params;
	x->file_path = file_path;
	x->directory_path = directory_path;
	x->type = type;
	x->pfn = &directories_monitor::send_to_observer;
	x->obj = this;
	auto y = static_cast<void*> (x);
	return y;
}

void directories_monitor::check_for_erase(int index) {
	std::unique_lock<std::mutex> lock(mutex);
	try {
		for (auto it = list_of_directories[index].paths.begin(); it != list_of_directories[index].paths.end(); ) {
			auto file = it->second;
			auto tmp = list_of_directories[index].parent_directory;
			tmp += file.cFileName;
			if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				tmp += '\\';
			}
			if (GetFileAttributes(tmp.c_str()) == INVALID_FILE_ATTRIBUTES) {
				this_file = it->first;
				directory_path = list_of_directories[index].parent_directory;
				if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					thread_pool.add_task({ build_params(this_file, 5, directory_path), send_to_observer_TP });
				}
				else {
					thread_pool.add_task({ build_params(this_file, 4, directory_path), send_to_observer_TP });
				}
				it = list_of_directories[index].paths.erase(it);
			}
			else
				++it;
		}
	}
	catch (...) {
		printf("\n ERROR: CheckForErase function failed.\n");
		ExitProcess(GetLastError());
	}
	lock.unlock();
}

void directories_monitor::check_for_modifing_and_adding(int index) {
	std::unique_lock<std::mutex> lock(mutex);
	try {
		auto tmp_path = list_of_directories[index].parent_directory;
		tmp_path += L"*.*";
		WIN32_FIND_DATA file;
		HANDLE hFind = FindFirstFile(tmp_path.c_str(), &file);
		do {
			if (std::wcscmp(file.cFileName, L".") != 0 && std::wcscmp(file.cFileName, L"..") != 0) {
				auto tmp = list_of_directories[index].parent_directory;
				tmp += file.cFileName;
				if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					tmp += '\\';
				}
				if (!contains(file.cFileName, index)) {
					this_file = file.cFileName;
					directory_path = list_of_directories[index].parent_directory;
					if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
						thread_pool.add_task({ build_params(this_file, 1, directory_path), send_to_observer_TP });
					}
					else {
						thread_pool.add_task({ build_params(this_file, 0, directory_path), send_to_observer_TP });
					}
					list_of_directories[index].paths[file.cFileName] = file;
				}
			}
		} while (FindNextFile(hFind, &file) != 0);
	}
	catch (...) {
		printf("\n ERROR: CheckForModifingAndAdding function failed.\n");
		ExitProcess(GetLastError());
	}
	lock.unlock();
}

std::vector<std::vector<HANDLE>> handles_split_by(const HANDLE* array, int split_size, int array_size) {
	std::vector<std::vector<HANDLE>> tmp_array; 
	for (int i = 0; i <= (array_size - 1) / split_size; i++) {
		tmp_array.push_back({});
		if ((i + 1) * split_size <= array_size) {
			for (int index = 0; index < split_size; index++) {
				tmp_array[i].push_back(array[i * split_size + index]);
			}
		}
		else {
			for (int index = 0; index < array_size - i * split_size; index++) {
				tmp_array[i].push_back(array[i * split_size + index]);
			}
		}
	}
	return tmp_array;
}

void directories_monitor::wait_for_notification() {
	thread_pool.Start();
	const int size_of_handle = 63;
	tmp_array = handles_split_by(dwChangeHandles, size_of_handle, list_of_directories.size());
	for (int i = 0; i <= (list_of_directories.size() - 1) / size_of_handle; i++) {
		tmp_array[i].push_back(EscapeEvent);
		auto function = [=]() {
			while (1) {
				DWORD dwWaitStatus = WaitForMultipleObjects(tmp_array[i].size(), tmp_array[i].data(), FALSE, INFINITE);
				if (dwWaitStatus < tmp_array[i].size() - 1) {
					//check for erase
					int index = i * size_of_handle + dwWaitStatus;

					check_for_erase(index);

					///check for modifing or adding
					check_for_modifing_and_adding(index);
					FindNextChangeNotification(tmp_array[i][dwWaitStatus]);
				}
				else {
					return;
				}
			}
		};
		threads.push_back(std::thread(function));
	}
}

void directories_monitor::start() {
	t_notification = std::thread(&directories_monitor::wait_for_notification, this);
}

directories_monitor::~directories_monitor() {
	for (auto& t : threads) {
		t.join();
	}
}
