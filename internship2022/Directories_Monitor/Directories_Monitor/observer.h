#pragma once
#include <string>
#include <iostream>
class observer {
private:
public:
	virtual void on_directory_changed(std::wstring file_path, int type, std::wstring& directory_path) = 0;
};