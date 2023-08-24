#include "SDK.h"

LONG ntOpenProcessHook(PHANDLE ProcessHandle, ACCESS_MASK AccessMask, POBJECT_ATTRIBUTES ObjectAttributes, CLIENT_ID* ClientID);

Security::Security() {
#ifdef _DEBUG
#else
	callDebgCheck->start(5000, [&]() {
		dbgFound = debuggerCheck->checking();
		if (dbgFound) {
			apiConnector->sendSecurityDbgInfo();
		}
		if (!hookCheck()) {
			apiConnector->sendSecurityDbgInfo();
		}
		}
	);
	//TODO App hash checksum need for more protection?
	callInfo->start(10000, [&]() {
		dto->secInfReq->hash = calcHash();
		if (apiConnector->sendSecurityInfo()) {
		}
		}
	);

	setupHooks();
#endif
}


Security::~Security() {
	deleteHooks();
}

bool Security::hookCheck() {
	OBF_BEGIN
		THREADINFOCLASS Info = (THREADINFOCLASS)0x26;
	auto chekHook = nt->pNtSetInformationThread((HANDLE)0xFFFFF, Info, NULL, 0);
	IF(NT_SUCCESS(V(chekHook))) {
		RETURN(false);
	}
	ENDIF
		RETURN(true);
	OBF_END

}

std::string Security::calcHash() {

	auto filePath = [] {
		char buffer[MAX_PATH];
		GetModuleFileNameA(NULL, buffer, MAX_PATH);
		std::string::size_type pos = std::string(buffer).find_last_of("\\/");
		return std::string(buffer);
		}();

		std::ifstream file(filePath, std::ios::binary);
		if (!file) {
			return "";
		}

		constexpr const std::size_t buffer_size{ 1 << 12 };
		char buffer[buffer_size];

		unsigned char hash[SHA256_DIGEST_LENGTH] = { 0 };

		SHA256_CTX ctx;
		SHA256_Init(&ctx);

		while (file.good()) {
			file.read(buffer, buffer_size);
			SHA256_Update(&ctx, buffer, (size_t)file.gcount());
		}
		SHA256_Final(hash, &ctx);
		file.close();

		std::ostringstream os;
		os << std::hex << std::setfill('0');

		for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
			os << std::setw(2) << static_cast<unsigned int>(hash[i]);
		}

		return os.str();
}


NTSTATUS ntOpenProcessHook(PHANDLE ProcessHandle, ACCESS_MASK AccessMask, POBJECT_ATTRIBUTES ObjectAttributes, CLIENT_ID* ClientID) {
	NTSTATUS orig = nt->pZwOpenProcess(ProcessHandle, AccessMask, ObjectAttributes, ClientID);

	HANDLE PID;
	__try {
		PID = ClientID->UniqueProcess;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		return STATUS_INVALID_PARAMETER;
	}

	if (GetCurrentProcess() == (HANDLE)PID)
	{
		return 0xc0000022;
	}

	return orig;
}

void Security::setupHooks() {

	DetourRestoreAfterWith();

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourAttach(&(PVOID&)nt->pZwOpenProcess, ntOpenProcessHook);

	LONG error = DetourTransactionCommit();
	if (error != NO_ERROR) {
		//Log md
	}
}

void Security::deleteHooks() {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&(PVOID&)nt->pZwOpenProcess, ntOpenProcessHook);

	LONG error = DetourTransactionCommit();
	if (error != NO_ERROR) {
		//Log md
	}
}


