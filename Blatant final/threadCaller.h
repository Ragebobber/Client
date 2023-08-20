#pragma once
using pNtSetInformationThread = LONG ( WINAPI* ) (IN HANDLE, IN THREADINFOCLASS, IN PVOID, IN ULONG);

class ThreadClaller {
public:
	ThreadClaller ( ):_execute ( false ) {};

	~ThreadClaller ( ) {
		if ( _execute.load ( std::memory_order_acquire ) ) {
			stop ( );
		};
	};


	void stop ( ) {
		_execute.store ( false, std::memory_order_release );
		if ( _thd.joinable ( ) )
			_thd.join ( );
	}

	void start ( int interval, std::function<void ( void )> func ) {
		if ( _execute.load ( std::memory_order_acquire ) ) {
			stop ( );
		};
		_execute.store ( true, std::memory_order_release );
		_thd = std::thread ( [this, interval, func]( ) {
			while ( _execute.load ( std::memory_order_acquire ) ) {
				func ( );
				std::this_thread::sleep_for (
					std::chrono::milliseconds ( interval ) );
			}
			} );
#ifdef _DEBUG
#else
		hideThread ( _thd );
#endif
	}

	bool is_running ( ) const noexcept {
		return (_execute.load ( std::memory_order_acquire ) &&
			_thd.joinable ( ));
	}

private:
	std::atomic<bool> _execute;
	std::thread _thd;
private:
	void hideThread ( std::thread &thd ) {

		HMODULE ntDll = GetModuleHandle ( S_INJ_NTDLL );
		if ( !ntDll )
			return;
		pNtSetInformationThread ntSetInformationThread = (pNtSetInformationThread) GetProcAddress ( ntDll, S_INJ_SET_INFO_THREAD );
		if ( !ntSetInformationThread )
			return;

		THREADINFOCLASS hide = (THREADINFOCLASS) 0x11;

		auto status = ntSetInformationThread ( thd.native_handle(), hide, NULL, 0 );

		if ( !NT_SUCCESS ( status ) ) {
			return;
		}
	}
};