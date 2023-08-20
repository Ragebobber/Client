#pragma once

class Hwid {
public:
	Hwid ( );
	~Hwid ( );

	std::string	GetHashSerialKey ( );

private:
	std::string	GetHwUID ( );
	DWORD		GetVolumeID ( );
	std::string	GetCompUserName ( bool User );
	std::string	GetSerialKey ( );
	std::string StringToHex ( const std::string input );
	std::string md5 ( const std::string& str );
};

