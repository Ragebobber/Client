#pragma once

typedef long (NTAPI* pNtOpenProcess)(
	PHANDLE            ProcessHandle,
	ACCESS_MASK        AccessMask,
	POBJECT_ATTRIBUTES ObjectAttributes,
	CLIENT_ID* ClientID
	);

static pNtOpenProcess o_ntOpenProcess = 0;



class Security {
public:
	Security();
	~Security();

private:
	DebuggerCheck* debuggerCheck = new DebuggerCheck();
	bool dbgFound = false;
private:
	bool hookCheck();
	std::string calcHash();
	void setupHooks();
	void deleteHooks();
	ThreadClaller* callDebgCheck = new ThreadClaller();
	ThreadClaller* callInfo = new ThreadClaller();

};
