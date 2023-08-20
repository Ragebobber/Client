#pragma once

struct LoginRequest {
	std::string login;
	std::string password;
    std::string hwid = std::shared_ptr<Hwid> ( ) -> GetHashSerialKey();

    std::string convertToAuthJSON ( ) {
        json::value jv = {
            {S_DTO_PASSWORD,password},
            {S_DTO_LOGIN,login},
            {S_DTO_HWID,hwid}
        };
        return  json::serialize ( jv );
    }
};