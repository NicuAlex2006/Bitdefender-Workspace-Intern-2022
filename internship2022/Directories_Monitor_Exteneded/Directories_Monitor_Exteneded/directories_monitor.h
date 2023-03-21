#pragma once
#include <string>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <filesystem>
#include <windows.h>
#include <iostream>
#include <vector>
#include <mutex>
#include <future>
#include <thread>
#include "observer.h"
#include "thread_pool.h"

class directories_monitor;

typedef void(directories_monitor::* pfn_send_to_observer)(std::wstring, int type, std::wstring);

struct directories {
	std::wstring parent_directory;
	std::unordered_map<std::wstring, WIN32_FIND_DATA> paths;
};

struct params {
	std::wstring file_path, directory_path;
	int type;
	directories_monitor* obj;
	pfn_send_to_observer pfn;
};

class WaitForObjects {
public:
	int WaitForMultipleObjects(DWORD size, const HANDLE* array);
private:
};

class directories_monitor {
public:
	directories_monitor(const std::vector<std::wstring>& path_to_watch);
	void start();
	void add_observer(observer* obs);
	void send_to_observer(std::wstring file_path, int type, std::wstring directory_path);
	void check_for_modifing_and_adding(int index);
	void check_for_erase(int index);
	void stop();
	void wait_for_notification();
	void* build_params(std::wstring file_path, int type, std::wstring directory_path);
	std::vector<directories> list_of_directories;
	~directories_monitor();
private:
	TP thread_pool;
	WaitForObjects wait;
	std::wstring this_file, directory_path;
	std::thread t_notification;
	std::mutex mutex;
	bool contains(const std::wstring& key, int index);
	std::vector<observer*> observers_list;
	HANDLE* dwChangeHandles;
	std::vector<std::thread> threads;
	std::vector<std::vector<HANDLE>> tmp_array;
	HANDLE EscapeEvent = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		TEXT("EscapeEvent")
	);
};
