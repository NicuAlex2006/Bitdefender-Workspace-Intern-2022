#pragma once
#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)
EXTERN_DLL_EXPORT int copy_directory(const wchar_t* folder_to_copy, const wchar_t* folder_destination);
void copyFile_(const wchar_t* inDir, const wchar_t* outDir);
void print(const wchar_t* s);