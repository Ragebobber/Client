#pragma once

struct LoginResponse {

    bool convertFromJSOM(std::shared_ptr<HttpStruct> response) {
        if (!response->error.empty() || response->body.empty())
            return false;
        json::value jv = json::parse(response->body);
        if (jv.at(S_DTO_TOKEN).is_string()) {
            response->token = json::value_to< std::string>(jv.at(S_DTO_TOKEN));
            return true;
        }
        return false;
    }
};