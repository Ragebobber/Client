#pragma once

struct DllResponse {

	std::vector<BYTE> convertFromJSOM(std::shared_ptr<HttpStruct> response) {
		if (!response->error.empty() || response->body.empty())
			return {};

		json::value jv = json::parse(response->body);
		//clear 
		response->body.clear();

		std::string dllDataStr = json::value_to<std::string>(jv.at(S_DTO_DATA));
		std::vector<BYTE> ret;
		ret.resize(beast::detail::base64::decoded_size(dllDataStr.size()));
		std::size_t read = beast::detail::base64::decode(ret.data(), dllDataStr.c_str(), dllDataStr.size()).first;
		ret.resize(read);
		return ret;
	}
};