#include<iostream>
#include<thread>
#include<Windows.h>

int main() {
	HANDLE Stop_Thread = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		NULL
	);
	auto function = [=]() {
		WaitForSingleObject(Stop_Thread, INFINITE);
	};
	std::thread t(function);
	std::string s;
	std::cin >> s;
	SetEvent(Stop_Thread);
	if (t.joinable())
		t.join();
	return 0;
}