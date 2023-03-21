#include "clients.h"

void client_A::on_directory_changed(std::wstring file_path, int type, std::wstring& directory_type) {
	if (type % 2 == 0)
		std::cout << "File ";
	else {
		std::cout << "Folder ";
	}
	if (type / 2 == 0) {
		std::cout << "created ";
	}
	else if (type / 2 == 1) {
		std::cout << "modifed ";
	}
	else {
		std::cout << "deleted ";
	}
	std::wcout << "in " << directory_type << ": ";
	std::wcout << file_path << "\n";
}