#pragma once

class RestClient {
public:
	explicit RestClient(const std::string& host, const std::string& port);
	~RestClient();
	std::shared_ptr<HttpStruct> getRequest(const std::string& endpoint);
	std::shared_ptr<HttpStruct>  postRequest(std::string endPoint, std::string jsonBody);
private:
	std::string host_;
	std::string port_;
private:
	void closeStream(beast::ssl_stream<beast::tcp_stream>&);
	http::response<http::string_body> writeHttpReq(beast::ssl_stream<beast::tcp_stream>&, http::request<http::string_body>&);
};

