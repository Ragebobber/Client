#include "SDK.h"
// thx Indigo 
//zamanback im my hearth
Hwid::Hwid ( ) {
}

Hwid::~Hwid ( ) {
}


std::string Hwid::GetHwUID ( ) {
	HW_PROFILE_INFO hwProfileInfo;
	std::string szHwProfileGuid = "";

	if ( GetCurrentHwProfileA ( &hwProfileInfo ) != NULL )
		szHwProfileGuid = hwProfileInfo.szHwProfileGuid;

	return szHwProfileGuid;
}

DWORD Hwid::GetVolumeID ( ) {
	DWORD VolumeSerialNumber;

	BOOL GetVolumeInformationFlag = GetVolumeInformationA (
		"C:\\",
		NULL,
		NULL,
		&VolumeSerialNumber,
		NULL,
		NULL,
		NULL,
		NULL
	);

	if ( GetVolumeInformationFlag )
		return VolumeSerialNumber;

	return 0;
}

std::string Hwid::GetCompUserName ( bool User ) {
	std::string CompUserName = "";

	char szCompName[MAX_COMPUTERNAME_LENGTH + 1];
	char szUserName[MAX_COMPUTERNAME_LENGTH + 1];

	DWORD dwCompSize = sizeof ( szCompName );
	DWORD dwUserSize = sizeof ( szUserName );

	if ( GetComputerNameA ( szCompName, &dwCompSize ) ) {
		CompUserName = szCompName;

		if ( User && GetUserNameA ( szUserName, &dwUserSize ) ) {
			CompUserName = szUserName;
		}
	}

	return CompUserName;
}

std::string Hwid::GetSerialKey ( ) {
	std::string SerialKey = "61A345B5496B2";
	std::string CompName = GetCompUserName ( false );
	std::string UserName = GetCompUserName ( true );

	SerialKey.append ( StringToHex ( GetHwUID ( ) ) );
	SerialKey.append ( "-" );
	SerialKey.append ( StringToHex ( std::to_string ( GetVolumeID ( ) ) ) );
	SerialKey.append ( "-" );
	SerialKey.append ( StringToHex ( CompName ) );
	SerialKey.append ( "-" );
	SerialKey.append ( StringToHex ( UserName ) );

	return SerialKey;
}

std::string Hwid::GetHashSerialKey ( ) {
	auto serial = GetSerialKey ( );
	return md5 ( serial );
}

std::string Hwid::StringToHex ( const std::string input ) {
	const char* lut = "0123456789ABCDEF";
	size_t len = input.length ( );
	std::string output = "";

	output.reserve ( 2 * len );

	for ( size_t i = 0; i < len; i++ ) {
		const unsigned char c = input[i];
		output.push_back ( lut[c >> 4] );
		output.push_back ( lut[c & 15] );
	}

	return output;
}

std::string Hwid::md5 ( const std::string& str ) {
	unsigned char hash[MD5_DIGEST_LENGTH];

	MD5_CTX md5;
	MD5_Init ( &md5 );
	MD5_Update ( &md5, str.c_str ( ), str.size ( ) );
	MD5_Final ( hash, &md5 );

	std::stringstream ss;

	for ( int i = 0; i < MD5_DIGEST_LENGTH; i++ ) {
		ss << std::hex << std::setw ( 2 ) << std::setfill ( '0' ) << static_cast<int>(hash[i]);
	}
	return ss.str ( );
}