#pragma once
#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)
typedef void(*pfn_on_enum_files)(const wchar_t*, const wchar_t*, void* ctx);
int enumerate(const wchar_t* folder, pfn_on_enum_files callback, void* ctx);
EXTERN_DLL_EXPORT int enumerate(const wchar_t* folder);