#pragma once

struct SecurityInfo {
    std::string hash;

    std::string convertToAuthJSON() {
        json::value jv = {
            {S_DTO_HASH,hash},
        };
        return  json::serialize(jv);
    }
};