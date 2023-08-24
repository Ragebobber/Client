#pragma once
#define RELOC_FLAG32(RelInfo) ((RelInfo >> 0x0C) == IMAGE_REL_BASED_HIGHLOW)
#define RELOC_FLAG RELOC_FLAG32
#define THREAD_CREATE_FLAGS_CREATE_SUSPENDED 0x00000001
#define THREAD_CREATE_FLAGS_SKIP_THREAD_ATTACH 0x00000002 
#define THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER 0x00000004
#define THREAD_CREATE_FLAGS_HAS_SECURITY_DESCRIPTOR 0x00000010
#define THREAD_CREATE_FLAGS_ACCESS_CHECK_IN_TARGET 0x00000020 
#define THREAD_CREATE_FLAGS_INITIAL_THREAD 0x00000080


using f_LoadLibraryA = HINSTANCE(WINAPI*)(const char* lpLibFilename);
using f_GetProcAddress = FARPROC(WINAPI*)(HMODULE hModule, LPCSTR lpProcName);
using f_DLL_ENTRY_POINT = BOOL(WINAPI*)(void* hDll, DWORD dwReason, void* pReserved);
using f_GetModuleHandle = HMODULE(WINAPI*)(LPCSTR lpModuleName);
using f_MultiByteToWideChar = INT(NTAPI*)(UINT CodePage, DWORD dwFlags, _In_NLS_string_(cbMultiByte)LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR  lpWideCharStr, int  cchWideChar);


struct MANUAL_MAPPING_DATA {
	f_LoadLibraryA pLoadLibraryA;
	f_GetProcAddress pGetProcAddress;
	f_GetModuleHandle pGetModuleHandle;
	BYTE* pbase;
	DWORD fdwReasonParam;
	LPVOID reservedParam;
	HINSTANCE hMod;
};

class Injection {
public:
	Injection();
	~Injection();

	void setPrivileges();

	DWORD getProcessIdByName(const std::string& processName);

	bool inject(Sub* sub);

private:
	LPVOID  _ntAllocateVirtualMemory(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
	HANDLE _ntCreateThreadEx(HANDLE hProcess, LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId);
	HANDLE _ntOpenProcess(DWORD pid);
	BOOL _ntWriteVirtualMemory(HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesWritten);
};

