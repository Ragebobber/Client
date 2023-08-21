#include "SDK.h"

#pragma runtime_checks( "", off )
#pragma optimize( "", off )
#pragma code_seg(".0x16DC")
void  __stdcall  Shellcode ( MANUAL_MAPPING_DATA* pData ) {
	if ( !pData ) {
		return;
	}

	BYTE* pBase = pData->pbase;

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER) pBase;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS) ((BYTE*) pDosHeader + pDosHeader->e_lfanew);
	PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER) &pNtHeader->FileHeader;
	PIMAGE_OPTIONAL_HEADER pOptHeader = (PIMAGE_OPTIONAL_HEADER) &pNtHeader->OptionalHeader;

	auto _LoadLibraryA = pData->pLoadLibraryA;
	auto _GetProcAddress = pData->pGetProcAddress;
	auto _LdrLoadDll = pData->pLdrLoadDll;
	//auto _RtlInitUnicodeString = pData->pRtlInitUnicodeString;
	auto _GetModuleHandle = pData->pGetModuleHandle;
	auto _RtlInitAnsiString = pData->pRtlInitAnsiString;
	auto _RtlAnsiStringToUnicodeString = pData->pRtlAnsiStringToUnicodeString;
	auto _LdrGetProcedureAddress = pData->pLdrGetProcedureAddress;
	
	auto _DllMain = (f_DLL_ENTRY_POINT) (pBase + pOptHeader->AddressOfEntryPoint);

	//Relocate Image
	BYTE* LocationDelta = pBase - pOptHeader->ImageBase;
	if ( LocationDelta ) {

		auto* pRelocDir = (IMAGE_DATA_DIRECTORY*) (&pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC]);

		if ( !pRelocDir->Size ) {
			return;
		}

		auto* pRelocData = (IMAGE_BASE_RELOCATION*) (pBase + pRelocDir->VirtualAddress);

		while ( pRelocData->VirtualAddress ) {
			WORD* pRelativeInfo = (WORD*) (pRelocData + 1);
			UINT RelocCount = (pRelocData->SizeOfBlock - sizeof ( IMAGE_BASE_RELOCATION )) / sizeof ( WORD );

			for ( UINT i = 0; i < RelocCount; ++i, ++pRelativeInfo ) {
				if ( RELOC_FLAG ( *pRelativeInfo ) ) {
					UINT_PTR* pPatch = (UINT_PTR*) (pBase + pRelocData->VirtualAddress + ((*pRelativeInfo) & 0xFFF));
					*pPatch += (UINT_PTR) (LocationDelta);
				}
			}

			pRelocData = (IMAGE_BASE_RELOCATION*) ((BYTE*) (pRelocData) +pRelocData->SizeOfBlock);

			if ( pRelocData >= (IMAGE_BASE_RELOCATION*) (pBase + pRelocDir->VirtualAddress + pRelocDir->Size) ) {
				break;
			}

		}

		pOptHeader->ImageBase += (ULONG_PTR) (LocationDelta);
	}

	//remove strings from the import directory
	DWORD importSize = pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;
	if ( importSize ) {
		auto* pImportDescr = (IMAGE_IMPORT_DESCRIPTOR*) (pBase + pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
		while ( pImportDescr->Name ) {
			char* szMod = (char*) (pBase + pImportDescr->Name);

		/*
			ANSI_STRING astr = { 0 };
			UNICODE_STRING ustr = { 0 };

			_RtlInitAnsiString ( &astr, szMod );
			_RtlAnsiStringToUnicodeString ( &ustr, &astr, TRUE );

			HMODULE hDll = 0;
			LPVOID addr;
			_LdrLoadDll ( NULL, 0, &ustr, &hDll );
			_LdrGetProcedureAddress ( hDll, &astr, 0, &addr );*/
		
			HINSTANCE hDll = _LoadLibraryA ( szMod  );


			ULONG_PTR* pThunkRef = (ULONG_PTR*) (pBase + pImportDescr->OriginalFirstThunk);
			ULONG_PTR* pFuncRef = (ULONG_PTR*) (pBase + pImportDescr->FirstThunk);

			if ( !pThunkRef )
				pThunkRef = pFuncRef;

			for ( ; *pThunkRef; ++pThunkRef, ++pFuncRef ) {
				if ( IMAGE_SNAP_BY_ORDINAL ( *pThunkRef ) ) {
					*pFuncRef = (ULONG_PTR) _GetProcAddress ( hDll, (char*) (*pThunkRef & 0xFFFF) );
				}
				else {
					auto* pImport = (IMAGE_IMPORT_BY_NAME*) (pBase + (*pThunkRef));
					*pFuncRef = (ULONG_PTR) _GetProcAddress ( hDll, pImport->Name );
				}
			}
			++pImportDescr;
		}
		pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress = 0;
		pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size = 0;
	}
	//Fix tls
	DWORD tlsSize = pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size;
	if ( tlsSize ) {
		auto* pTLS = (IMAGE_TLS_DIRECTORY*) (pBase + pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);
		auto* pCallback = (PIMAGE_TLS_CALLBACK*) (pTLS->AddressOfCallBacks);
		for ( ; pCallback && *pCallback; ++pCallback )
			(*pCallback)(pBase, DLL_PROCESS_ATTACH, nullptr);

		pTLS->AddressOfCallBacks = 0;
		pTLS->AddressOfIndex = 0;
		pTLS->EndAddressOfRawData = 0;
		pTLS->SizeOfZeroFill = 0;
		pTLS->StartAddressOfRawData = 0;
		pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress = 0;
		pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size = 0;
	}

	_DllMain ( pBase, pData->fdwReasonParam, pData->reservedParam );

	for ( UINT i = 0; i < 0x1000; i += sizeof ( ULONG64 ) ) {
		*(ULONG64*) (pBase + i) = 0;
	}

	pData->hMod = (HINSTANCE) pBase;
}

#pragma code_seg(push,r1)      // stored in mySellCodeData
void __stdcall stub ( ) {
	int a = 0;
	return;
}
#pragma runtime_checks( "", restore )
#pragma optimize( "", on )

Injection::Injection ( ) {
	setPrivileges ( );
}

Injection::~Injection ( ) {
}

void Injection::setPrivileges ( ) {
	TOKEN_PRIVILEGES priv = { 0 };
	HANDLE hToken = NULL;
	if ( OpenProcessToken ( GetCurrentProcess ( ), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken ) ) {
		priv.PrivilegeCount = 1;
		priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if ( LookupPrivilegeValue ( NULL, SE_DEBUG_NAME, &priv.Privileges[0].Luid ) )
			AdjustTokenPrivileges ( hToken, FALSE, &priv, 0, NULL, NULL );

		CloseHandle ( hToken );
	}
}

DWORD  Injection::getProcessIdByName ( const std::string& processName ) {
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof ( PROCESSENTRY32 );

	HANDLE snapshot = CreateToolhelp32Snapshot ( TH32CS_SNAPPROCESS, NULL );

	if ( Process32First ( snapshot, &entry ) == TRUE ) {
		while ( Process32Next ( snapshot, &entry ) == TRUE ) {
			if ( entry.szExeFile == processName ) {
				CloseHandle ( snapshot );
				return entry.th32ProcessID;
			}
		}
	}

	CloseHandle ( snapshot );
	return 0;
}




LPVOID  Injection::_ntAllocateVirtualMemory ( HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD  flAllocationType , DWORD  flProtect ) {

	LPVOID ret = lpAddress;

	auto status = nt->pNtAllocateVirtualMemory ( hProcess, &ret, 0, &dwSize, flAllocationType, flProtect );

	if ( NT_SUCCESS( status ))//STATUS_SUCCESS
		return ret;

	return NULL;

}

HANDLE Injection::_ntCreateThreadEx ( HANDLE  hProcess, LPSECURITY_ATTRIBUTES  lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId ) {
	HANDLE threadHandle; 
	
	auto status = nt->pNtCreateThreadEx ( &threadHandle, 0x1FFFFF, NULL, hProcess,  lpStartAddress, lpParameter, THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER, NULL, NULL, NULL, NULL );

	if ( status == 0x00000000 ) {
		return threadHandle;
	}

	return 0;
}


HANDLE Injection::_ntOpenProcess ( DWORD pid ) {

	OBJECT_ATTRIBUTES objectAttributes;
	HANDLE handle;
	CLIENT_ID clientId;

	clientId.UniqueThread = NULL;
	clientId.UniqueProcess = (PVOID) (pid);
	InitializeObjectAttributes ( &objectAttributes, NULL, 0, NULL, NULL );

	auto status = nt->pZwOpenProcess ( &handle,
		PROCESS_ALL_ACCESS,
		&objectAttributes,
		&clientId );

	if ( !handle || !NT_SUCCESS(status)) {
		return 0;
	}
	return handle;
}

BOOL Injection::_ntWriteVirtualMemory ( HANDLE  hProcess, LPVOID  lpBaseAddress, LPCVOID lpBuffer, SIZE_T  nSize, SIZE_T* lpNumberOfBytesWritten ) {
	
	auto status = nt->pNtWriteVirtualMemory ( hProcess, lpBaseAddress, (PVOID)lpBuffer, nSize, lpNumberOfBytesWritten );

	if ( !NT_SUCCESS ( status ) )
		return FALSE;

	return TRUE;
}

bool  Injection::inject ( Sub* sub ) {

	auto name = sub->product->exeName;

	if(name.empty() )
		return false;

	DWORD pid = getProcessIdByName ( name );

	if ( !pid )
		return false;

	HANDLE hProc = _ntOpenProcess ( pid );

	if ( !hProc )
		return false;

	std::vector<BYTE> dllData = apiConnector->getDllData ( sub->id );

	if ( dllData.empty ( ) )
		return false;

	BYTE* pSrcData = dllData.data();

	dllData.clear ( );

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER) pSrcData;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS) ((BYTE*) pDosHeader + pDosHeader->e_lfanew);
	PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER) &pNtHeader->FileHeader;
	PIMAGE_OPTIONAL_HEADER pOptHeader = (PIMAGE_OPTIONAL_HEADER) &pNtHeader->OptionalHeader;
	PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION ( pNtHeader );

	if ( pDosHeader->e_magic != 0x5A4D ) {
		//std::cout << "Incorrect file";
		memset ( pSrcData, 0, sizeof ( pSrcData ) );
		return false;
	}

	// Allocating memory for the DLL
	BYTE* pTargetBase = (BYTE*) (_ntAllocateVirtualMemory ( hProc, nullptr, pOptHeader->SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE ));
	if ( !pTargetBase ) {
		//std::cout << "Error allocating memory for the DLL";
		memset ( pSrcData, 0, sizeof ( pSrcData ) );
		return false;
	}

	DWORD oldp = 0;
	auto changeProtect = VirtualProtectEx ( hProc, pTargetBase, pOptHeader->SizeOfImage, PAGE_EXECUTE_READWRITE, &oldp );
	if ( changeProtect == 0 ) {
		//std::cout << "Error change protect";
		memset ( pSrcData, 0, sizeof ( pSrcData ) );
		return false;
	}

	MANUAL_MAPPING_DATA data { 0 };
	data.pLoadLibraryA = LoadLibraryA;
	data.pGetProcAddress = GetProcAddress;
	data.pGetModuleHandle = GetModuleHandle;
	data.pLdrLoadDll = nt->pLdrLoadDll;
	//data.pRtlInitUnicodeString = nt->pRtlInitUnicodeString;
	data.pbase = pTargetBase;
	data.fdwReasonParam = DLL_PROCESS_ATTACH;
	data.reservedParam = 0;
	data.pRtlInitAnsiString = nt->pRtlInitAnsiString;
	data.pRtlAnsiStringToUnicodeString = nt->pRtlAnsiStringToUnicodeString;
	data.pLdrGetProcedureAddress = nt->pLdrGetProcedureAddress;


	// Copy the headers to target process
	auto writeHeaders = _ntWriteVirtualMemory ( hProc, pTargetBase, pSrcData, pOptHeader->SizeOfHeaders, nullptr );

	if ( writeHeaders == 0 ) {
		//std::cout << "Error write headers";
		VirtualFreeEx ( hProc, pTargetBase, 0, MEM_RELEASE );
		memset ( pSrcData, 0, sizeof ( pSrcData ) );
		return false;
	}

	// Copying sections of the dll to the target process
	for ( UINT i = 0; i != pFileHeader->NumberOfSections; ++i, ++pSectionHeader ) {
		if ( pSectionHeader->SizeOfRawData ) {
			auto cpySections = _ntWriteVirtualMemory ( hProc, pTargetBase + pSectionHeader->VirtualAddress, pSrcData + pSectionHeader->PointerToRawData, pSectionHeader->SizeOfRawData, nullptr );
			if ( cpySections == 0 ) {
			//	std::cout << "Error write sections" << pSectionHeader->Name;
				VirtualFreeEx ( hProc, pTargetBase, 0, MEM_RELEASE );
				memset ( pSrcData, 0, sizeof ( pSrcData ) );
				return false;
			}
		}
	}

	// Allocating memory for loader params
	BYTE* mappingParamsAlloc = (BYTE*) (_ntAllocateVirtualMemory ( hProc, nullptr, sizeof ( MANUAL_MAPPING_DATA ), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE ));
	if ( !mappingParamsAlloc ) {
		//std::cout << "Error allocating memory for loader params";
		VirtualFreeEx ( hProc, pTargetBase, 0, MEM_RELEASE );
		memset ( pSrcData, 0, sizeof ( pSrcData ) );
		return false;
	}

	//Write loader params
	auto writeLoaderParams = _ntWriteVirtualMemory ( hProc, mappingParamsAlloc, &data, sizeof ( MANUAL_MAPPING_DATA ), nullptr );
	if ( mappingParamsAlloc == 0 ) {
		//std::cout << "Error write loader params";
		VirtualFreeEx ( hProc, pTargetBase, 0, MEM_RELEASE );
		VirtualFreeEx ( hProc, mappingParamsAlloc, 0, MEM_RELEASE );
		delete[] pSrcData;
		return false;
	}

	BYTE* shellCodeBase = (BYTE*) Shellcode;
	BYTE* endShellCode = (BYTE*) stub;
	DWORD shellSize = (DWORD) endShellCode - (DWORD) shellCodeBase;

	// Allocating memory for the loader code.
	void* pShellcode = _ntAllocateVirtualMemory ( hProc, nullptr, shellSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE );
	if ( !pShellcode ) {
		//std::cout << "Error allocating memory for the loader code";
		VirtualFreeEx ( hProc, pTargetBase, 0, MEM_RELEASE );
		VirtualFreeEx ( hProc, mappingParamsAlloc, 0, MEM_RELEASE );
		memset ( pSrcData, 0, sizeof ( pSrcData ) );
		return false;
	}

	//Write loader code
	auto writeShellCode = _ntWriteVirtualMemory ( hProc, pShellcode, shellCodeBase, 4096, nullptr );
	if ( writeShellCode == 0 ) {
		VirtualFreeEx ( hProc, pTargetBase, 0, MEM_RELEASE );
		VirtualFreeEx ( hProc, mappingParamsAlloc, 0, MEM_RELEASE );
		VirtualFreeEx ( hProc, pShellcode, 0, MEM_RELEASE );
		memset ( pSrcData, 0, sizeof ( pSrcData ) );
		return false;
	}

	HANDLE hThread = _ntCreateThreadEx ( hProc, nullptr, 0, (LPTHREAD_START_ROUTINE) pShellcode, mappingParamsAlloc, 0, nullptr );
	
	if ( !hThread ) {
		VirtualFreeEx ( hProc, pTargetBase, 0, MEM_RELEASE );
		VirtualFreeEx ( hProc, mappingParamsAlloc, 0, MEM_RELEASE );
		VirtualFreeEx ( hProc, pShellcode, 0, MEM_RELEASE );
		memset ( pSrcData, 0, sizeof ( pSrcData ) );
		return false;
	}

	CloseHandle ( hThread );
	Sleep ( 10 );
	memset ( pSrcData, 0, sizeof ( pSrcData ) );


	//CLEAR PE HEAD
	BYTE* emptyBuffer = (BYTE*) malloc ( pOptHeader->SizeOfHeaders );
	if ( emptyBuffer != 0 ) {
		memset ( emptyBuffer, 0, pOptHeader->SizeOfHeaders );
		auto clearHeader = _ntWriteVirtualMemory ( hProc, pTargetBase, emptyBuffer, pOptHeader->SizeOfHeaders, nullptr );
		if ( clearHeader == 0 ) {
			//std::cout << "WARNING!: Can't clear HEADER\n";
		}

		
	}

	//ClearSections
	/*for ( UINT i = 0; i != pFileHeader->NumberOfSections; ++i, ++pSectionHeader ) {
		if ( pSectionHeader->Misc.VirtualSize ) {
			if ( strcmp ( (char*) pSectionHeader->Name, ".pdata" ) == 0 ||
				strcmp ( (char*) pSectionHeader->Name, ".rsrc" ) == 0 ||
				strcmp ( (char*) pSectionHeader->Name, ".reloc" ) == 0 ) {
				auto writeSections = WriteProcessMemory ( hProc, pTargetBase + pSectionHeader->VirtualAddress, emptyBuffer, pSectionHeader->Misc.VirtualSize, nullptr );
				if ( writeSections == 0 )
					std::cout << "Can't clear section" << pSectionHeader->Name << GetLastError ( );
			}
		}
	}*/
	
	//Protection
	for ( UINT i = 0; i != pFileHeader->NumberOfSections; ++i, ++pSectionHeader ) {
		if ( pSectionHeader->Misc.VirtualSize ) {
			DWORD old = 0;
			DWORD newP = PAGE_READONLY;

			if ( (pSectionHeader->Characteristics & IMAGE_SCN_MEM_WRITE) > 0 ) {
				newP = PAGE_READWRITE;
			}
			else if ( (pSectionHeader->Characteristics & IMAGE_SCN_MEM_EXECUTE) > 0 ) {
				newP = PAGE_EXECUTE_READ;
			}
			if ( VirtualProtectEx ( hProc, pTargetBase + pSectionHeader->VirtualAddress, pSectionHeader->Misc.VirtualSize, newP, &old ) ) {
			
				//std::cout << "section" << (char*) pSectionHeader->Name <<"set as" << newP;
			}
			else {
				//std::cout << "FAIL: section" << (char*) pSectionHeader->Name << "not set as" << newP;
			}
		}
	}
	DWORD old = 0;
	VirtualProtectEx ( hProc, pTargetBase, pSectionHeader->VirtualAddress, PAGE_READONLY, &old );


	CloseHandle ( hProc );
	return true;
}
