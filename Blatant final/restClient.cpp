#include "SDK.h"


RestClient::RestClient(const std::string& host, const std::string& port) : host_(host), port_(port)
{

}

RestClient::~RestClient() {
}

std::shared_ptr<HttpStruct> RestClient::getRequest(const std::string& endpoint) {
	try {
		net::io_context ioc;
		ssl::context ctx{ ssl::context::tlsv13_client };
		beast::ssl_stream<beast::tcp_stream> stream{ ioc, ctx };

		ctx.set_default_verify_paths();
		ctx.set_verify_mode(ssl::verify_peer);
		ctx.set_verify_callback([](bool preverified, boost::asio::ssl::verify_context& ctx) {
			// Add any custom verification logic if needed
			return preverified;
			});
		tcp::resolver resolver(ioc);
		// Set SNI Hostname (many hosts need this to handshake successfully)
		if (!SSL_set_tlsext_host_name(stream.native_handle(), host_.c_str()))
		{
			beast::error_code ec{ static_cast<int>(::ERR_get_error()), net::error::get_ssl_category() };
			throw beast::system_error{ ec };
		}
		// Look up the domain name
		auto const results = resolver.resolve(host_, port_);
		// Make the connection on the IP address we get from a lookup
		beast::get_lowest_layer(stream).connect(results);
		// Perform the SSL handshake
		stream.handshake(ssl::stream_base::client);
		// Set up an HTTP GET request message
		http::request<http::string_body> req{ http::verb::get, endpoint, 11 };
		req.set(http::field::host, host_);
		req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
		req.set(http::field::authorization, "Bearer " + httpStruct->token);

		// Declare a container to hold the response
		auto res = writeHttpReq(stream, req);

		// Gracefully close the stream
		closeStream(stream);

		beast::string_view reason = res.reason();
		// status code enum
		http::status result = res.result();

		// status code as integer
		int httpStatusCode = res.result_int();

		httpStruct->body = res.body();

		if (httpStatusCode != 200) {
			httpStruct->error = reason;
		}

		return httpStruct;
	}
	catch (boost::system::system_error e) {
		httpStruct->error = e.code().message();
		return nullptr;
	}
}

std::shared_ptr<HttpStruct> RestClient::postRequest(std::string endPoint, std::string jsonBody) {
	try {
		net::io_context ioc;
		ssl::context ctx{ ssl::context::tlsv13_client };
		beast::ssl_stream<beast::tcp_stream> stream{ ioc, ctx };
		ctx.set_default_verify_paths();
		ctx.set_verify_mode(ssl::verify_peer);
		ctx.set_verify_callback([](bool preverified, boost::asio::ssl::verify_context& ctx) {
			// Add any custom verification logic if needed
			return preverified;
			});

		// These objects perform our I/O
		tcp::resolver resolver(ioc);

		// Set SNI Hostname (many hosts need this to handshake successfully)
		if (!SSL_set_tlsext_host_name(stream.native_handle(), serverHost)) {
			beast::error_code ec{ static_cast<int>(::ERR_get_error()), net::error::get_ssl_category() };
			throw beast::system_error{ ec };
		}
		// Look up the domain name
		auto const results = resolver.resolve(serverHost, serverPort);
		// Make the connection on the IP address we get from a lookup
		beast::get_lowest_layer(stream).connect(results);

		// Perform the SSL handshake
		stream.handshake(ssl::stream_base::client);

		http::request<http::string_body> req{ http::verb::post, endPoint, 11 };
		req.set(http::field::host, serverHost);
		req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
		req.set(http::field::content_type, "application/json");
		req.set(http::field::content_length, boost::lexical_cast<std::string>(jsonBody.size()));
		req.set(http::field::authorization, "Bearer " + httpStruct->token);
		req.body() = jsonBody;

		// Declare a container to hold the response
		auto res = writeHttpReq(stream, req);
		// Gracefully close the stream
		closeStream(stream);

		beast::string_view reason = res.reason();

		// status code enum
		http::status result = res.result();

		// status code as integer
		int httpStatusCode = res.result_int();

		httpStruct->body = res.body();

		if (httpStatusCode != 200) {
			httpStruct->error = reason;
		}
		return httpStruct;
	}
	catch (boost::system::system_error e) {
		httpStruct->error = e.code().message();
		return nullptr;
	}
}

void RestClient::closeStream(beast::ssl_stream<beast::tcp_stream>& stream)
{
	beast::error_code ec;
	stream.shutdown(ec);
	stream.next_layer().close();
	if (ec == net::error::eof)
	{
		// Rationale:
		// http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
		ec = {};
	}
	if (ec)
		throw beast::system_error{ ec };
}

http::response<http::string_body> RestClient::writeHttpReq(beast::ssl_stream<beast::tcp_stream>& stream, http::request<http::string_body>& req)
{
	// Send the HTTP request to the remote host
	http::write(stream, req);

	// This buffer is used for reading and must be persisted
	beast::flat_buffer buffer;
	// Declare a container to hold the response
	http::response<http::string_body> res;

	// Receive the HTTP response
	http::read(stream, buffer, res);

	return res;
}

