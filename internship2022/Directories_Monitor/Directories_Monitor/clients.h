#pragma once
#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include "observer.h"
class client_A : public observer {
private:
public:
	void on_directory_changed(std::wstring file_path, int type, std::wstring& directory_path);
};