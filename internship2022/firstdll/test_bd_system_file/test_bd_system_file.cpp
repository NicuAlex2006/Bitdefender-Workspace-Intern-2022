// test_bd_system_file.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <filesystem>
#include <windows.h>
typedef int(pfn_on_enum_files)(const wchar_t*);
typedef int(pfn_on_copy_directory)(const wchar_t*, const wchar_t*);

int main()
{
	auto h = LoadLibrary(L"bd_system_file.dll");
	auto m_pfn_enumerate = reinterpret_cast<pfn_on_enum_files*>(
		GetProcAddress(h, "enumerate")
	);
	auto m_pfn_copy_directory = reinterpret_cast<pfn_on_copy_directory*> (
		GetProcAddress(h, "copy_directory")
	);
	if (m_pfn_enumerate(L"C:\\Users\\anicu\\git\\bdinternship\\internship2022\\") == 0)
		std::cout << "Enumerate succesful!\n";
	else
		std::cout << "Enumerate:Bad args!\n";
	std::cout << "-----------------------------------------------------------------------------------\n";
	std::cout << "-----------------------------------------------------------------------------------\n";
	std::cout << "-----------------------------------------------------------------------------------\n";
	std::cout << "-----------------------------------------------------------------------------------\n";
	int a = m_pfn_copy_directory(L"C:\\Users\\anicu\\git\\bdinternship\\internship2022\\", L"C:\\Users\\anicu\\Desktop\\");
	if (a == 0)
		std::cout << "Copy succesful!\n";
	else
		std::cout << "Copy:Bad args!\n";
	FreeLibrary(h);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
