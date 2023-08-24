#include "SDK.h"

DebuggerCheck::DebuggerCheck() {
}

DebuggerCheck::~DebuggerCheck() {
}

bool DebuggerCheck::checking() {
	OBF_BEGIN
		RETURN(isDebuggerCheckImpl() || isRemoteDebuggerPresent() || isNtGlobalFlag() || isMemEddit());
	OBF_END
}

bool DebuggerCheck::isDebuggerCheckImpl() {
	auto check = IsDebuggerPresent();
	PPEB pPEB = (PPEB)__readfsdword(0x30);
	return check == TRUE || pPEB->BeingDebugged != NULL;
}


bool DebuggerCheck::isRemoteDebuggerPresent() {
	OBF_BEGIN
		BOOL isDebuggerPresent = FALSE;
	CheckRemoteDebuggerPresent(GetCurrentProcess(), &isDebuggerPresent);


	DWORD _isDebuggerPresent = 0;
	auto check1 = nt->pNtQueryInformationProcess(GetCurrentProcess(), ProcessDebugPort, &_isDebuggerPresent, sizeof DWORD, NULL);

	HANDLE hProcessDebugObject = NULL;
	DWORD processDebugFlags = 0;

	auto check2 = nt->pNtQueryInformationProcess(GetCurrentProcess(), (PROCESSINFOCLASS)ProcessDebugObjectHandle, &hProcessDebugObject, sizeof HANDLE, NULL);
	auto check3 = nt->pNtQueryInformationProcess(GetCurrentProcess(), (PROCESSINFOCLASS)ProcessDebugFlags, &processDebugFlags, sizeof DWORD, NULL);

	RETURN(isDebuggerPresent == TRUE || _isDebuggerPresent != 0 || (hProcessDebugObject != NULL || processDebugFlags == 0));

	OBF_END
}

bool DebuggerCheck::isNtGlobalFlag() {
	DWORD pNtGlobalFlag = NULL;
	PPEB pPeb = (PPEB)__readfsdword(0x30);
	pNtGlobalFlag = *(PDWORD)((PBYTE)pPeb + 0x68);
	return (pNtGlobalFlag & 0x70) != 0;
}

bool DebuggerCheck::isForceFlags() {
	BOOL found = FALSE;
	_asm
	{
		xor eax, eax;			//clear the eax register
		mov eax, fs: [0x30] ;	//reference start of the process environment block
		mov eax, [eax + 0x02];	//beingdebugged is stored in peb + 2
		and eax, 0x000000FF;	//reference one byte
		mov found, eax;			//copy value to found
	}
	return found == 1;
}

bool DebuggerCheck::isMemEddit() {
	ULONG_PTR hits;
	DWORD granularity;

	PVOID* addresses = static_cast<PVOID*>(VirtualAlloc(NULL, 4096 * sizeof(PVOID), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
	if (addresses == NULL) {
		return true;
	}

	int* buffer = static_cast<int*>(VirtualAlloc(NULL, 4096 * 4096, MEM_RESERVE | MEM_COMMIT | MEM_WRITE_WATCH, PAGE_READWRITE));
	if (buffer == NULL) {
		VirtualFree(addresses, 0, MEM_RELEASE);
		return true;
	}

	//read the buffer once
	buffer[0] = 1234;

	hits = 4096;
	if (GetWriteWatch(0, buffer, 4096, addresses, &hits, &granularity) != 0) {
		return true;
	}
	else {
		//free the memory again
		VirtualFree(addresses, 0, MEM_RELEASE);
		VirtualFree(buffer, 0, MEM_RELEASE);

		//we should have 1 hit if everything is fine
		return (hits == 1) ? false : true;
	}
}

