#pragma once
#define ProcessDebugObjectHandle 0x1E
#define ProcessDebugFlags 0x1F

#define FLG_HEAP_ENABLE_TAIL_CHECK   0x10
#define FLG_HEAP_ENABLE_FREE_CHECK   0x20
#define FLG_HEAP_VALIDATE_PARAMETERS 0x40
#define NT_GLOBAL_FLAG_DEBUGGED (FLG_HEAP_ENABLE_TAIL_CHECK | FLG_HEAP_ENABLE_FREE_CHECK | FLG_HEAP_VALIDATE_PARAMETERS)



class DebuggerCheck {
public:
	DebuggerCheck();
	~DebuggerCheck();

	bool checking();

private:
	bool isDebuggerCheckImpl();
	bool isRemoteDebuggerPresent();
	bool isNtGlobalFlag();
	bool isForceFlags();
	bool isMemEddit();
};

