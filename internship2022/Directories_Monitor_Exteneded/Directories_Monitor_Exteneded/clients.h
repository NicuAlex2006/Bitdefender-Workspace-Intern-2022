#pragma once
#include <string>
#include <fstream>
#include <chrono>
#include <mutex>
#include "observer.h"
class client_A : public observer {
private:
	std::mutex mutex;
public:
	void on_directory_changed(std::wstring file_path, int type, std::wstring directory_path);
};