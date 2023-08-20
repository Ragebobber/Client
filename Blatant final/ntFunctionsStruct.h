#pragma once
using oNtSetInformationThread = LONG ( NTAPI* ) (IN HANDLE, IN THREADINFOCLASS, IN PVOID, IN ULONG);
using oZwOpenProcess = LONG ( NTAPI* )(PHANDLE ProcessHandle,ACCESS_MASK AccessMask,POBJECT_ATTRIBUTES ObjectAttributes,CLIENT_ID* ClientID);
using oNtAllocateVirtualMemory = LONG(NTAPI* )(
	HANDLE             ProcessHandle,
	PVOID* BaseAddress,
	ULONG              ZeroBits,
	PULONG             RegionSize,
	ULONG              AllocationType,
	ULONG              Protect
	);
using oNtCreateThreadEx = LONG (NTAPI* )(
	PHANDLE hThread,
	ACCESS_MASK DesiredAccess,
	PVOID ObjectAttributes,
	HANDLE ProcessHandle,
	PVOID lpStartAddress,
	PVOID lpParameter,
	ULONG Flags,
	SIZE_T StackZeroBits,
	SIZE_T SizeOfStackCommit,
	SIZE_T SizeOfStackReserve,
	PVOID lpBytesBuffer
	);
using oNtWriteVirtualMemory = LONG ( NTAPI* )(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToWrite, PULONG NumberOfBytesWritten);
using oLdrLoadDll = LONG ( NTAPI* )(PWCHAR PathToFile, ULONG Flags, PUNICODE_STRING ModuleFileName, HMODULE* ModuleHandle);
using oRtlInitUnicodeString = VOID ( NTAPI* )(PUNICODE_STRING DestinationString, PCWSTR SourceString);
using oNtQueryInformationProcess = LONG ( WINAPI* )(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);
using oRtlInitAnsiString = VOID ( NTAPI* )(PANSI_STRING DestinationString, PCSTR SourceString);
using oRtlAnsiStringToUnicodeString = NTSTATUS ( NTAPI* )(PUNICODE_STRING DestinationString, PCANSI_STRING   SourceString, BOOLEAN AllocateDestinationString);
using oLdrGetProcedureAddress = VOID ( NTAPI* )(IN HMODULE ModuleHandle,
	IN PANSI_STRING FunctionName OPTIONAL,
	IN WORD Ordinal OPTIONAL,
	OUT PVOID* FunctionAddress);
using oLoadLibraryA = HMODULE(WINAPI*)(LPCSTR lpLibFileName);

struct NtFunctions {
	HMODULE ntDll;
	oNtSetInformationThread pNtSetInformationThread = nullptr;
	oZwOpenProcess	pZwOpenProcess = nullptr;
	oNtAllocateVirtualMemory pNtAllocateVirtualMemory = nullptr;
	oNtCreateThreadEx pNtCreateThreadEx = nullptr;
	oNtWriteVirtualMemory pNtWriteVirtualMemory = nullptr;
	oLdrLoadDll pLdrLoadDll = nullptr;
	oRtlInitUnicodeString pRtlInitUnicodeString = nullptr;
	oNtQueryInformationProcess pNtQueryInformationProcess = nullptr;
	oRtlInitAnsiString pRtlInitAnsiString = nullptr;
	oRtlAnsiStringToUnicodeString pRtlAnsiStringToUnicodeString = nullptr;
	oLdrGetProcedureAddress pLdrGetProcedureAddress = nullptr;
	oLoadLibraryA pLoadLibraryA = nullptr;

	NtFunctions ( ) {
		ntDll = GetModuleHandle ( S_INJ_NTDLL );
		if ( !ntDll )
			return;

		pNtSetInformationThread = (oNtSetInformationThread) GetProcAddress ( ntDll, S_INJ_SET_INFO_THREAD );
		if( !pNtSetInformationThread )
			return;

		pZwOpenProcess = (oZwOpenProcess) GetProcAddress ( ntDll, S_INJ_OPEN_PROCESS );
		if ( !pZwOpenProcess )
			return;

		pNtAllocateVirtualMemory = (oNtAllocateVirtualMemory) GetProcAddress ( ntDll, S_INJ_ALLOC_MEM );
		if ( !pNtAllocateVirtualMemory )
			return;

		pNtCreateThreadEx = (oNtCreateThreadEx) GetProcAddress ( ntDll, S_INJ_CREATE_THREAD );
		if ( !pNtCreateThreadEx )
			return;

		pNtWriteVirtualMemory = (oNtWriteVirtualMemory) GetProcAddress ( ntDll, S_INJ_WRITE_MEM );
		if( !pNtWriteVirtualMemory )
			return;

		pLdrLoadDll = (oLdrLoadDll) GetProcAddress ( ntDll, S_INJ_LDR_LOAD_DLL );
		if(!pLdrLoadDll )
			return;

		pRtlInitUnicodeString = (oRtlInitUnicodeString) GetProcAddress ( ntDll, S_INJ_INIT_UNICODE_STR );
		if( !pRtlInitUnicodeString )
			return;

		pNtQueryInformationProcess = (oNtQueryInformationProcess) GetProcAddress ( ntDll, S_INJ_INIT_INFOPROCESS_STR );
		if ( !pNtQueryInformationProcess )
			return;
		pRtlInitAnsiString = (oRtlInitAnsiString) GetProcAddress ( ntDll, S_INJ_INIT_ANSY_STR );
		if(!pRtlInitAnsiString )
			return;

		pRtlAnsiStringToUnicodeString = (oRtlAnsiStringToUnicodeString) GetProcAddress ( ntDll, S_INJ_ANSY_TO_UNI );
		if ( !pRtlAnsiStringToUnicodeString )
			return;

		pLdrGetProcedureAddress = (oLdrGetProcedureAddress) GetProcAddress ( ntDll, S_INJ_GET_PROC_ADDR );
		if ( !pLdrGetProcedureAddress )
			return;

		pLoadLibraryA = LoadLibraryA;
		if(!pLoadLibraryA)
			return;
	}

};