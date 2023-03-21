#pragma once
#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <filesystem>
#include <windows.h>
#include <vector>
#include "observer.h"

struct directories {
	std::wstring parent_directory;
	std::unordered_map<std::wstring, WIN32_FIND_DATA> paths;
};

class directories_monitor {
private:
	std::vector<directories> list_of_directories;
	bool contains(const std::wstring& key, int index);
	std::vector<observer*> observers_list;
	HANDLE *dwChangeHandles;
public:
	directories_monitor(const std::vector<std::wstring>& path_to_watch);
	void start();
	void add_observer(observer* obs);
	void send_to_observer(std::wstring file_path, int type, std::wstring& directory_path);
	void check_for_modifing_and_adding(int index);
	void check_for_erase(int index);
	~directories_monitor();
};