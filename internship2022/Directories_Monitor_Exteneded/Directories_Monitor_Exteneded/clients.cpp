#include "clients.h"

std::ofstream cout("output.out");

void client_A::on_directory_changed(std::wstring file_path, int type, std::wstring directory_type) {
	std::unique_lock<std::mutex> lock(mutex);
	if (type % 2 == 0)
		cout << "File ";
	else {
		cout << "Folder ";
	}
	if (type / 2 == 0) {
		cout << "created ";
	}
	else if (type / 2 == 1) {
		cout << "modifed ";
	}
	else {
		cout << "deleted ";
	}
	std::string dt(directory_type.begin(), directory_type.end());
	std::string fp(file_path.begin(), file_path.end());
	cout << "in " << dt << ": ";
	cout << fp << "\n";
	lock.unlock();
}