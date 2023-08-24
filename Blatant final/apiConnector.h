#pragma once

#define serverHost "localhost"
#define serverPort "8080"

class ApiConnector {
public:
	ApiConnector();
	~ApiConnector();

	bool login();
	std::map<long, std::string> getUserSubs();
	Sub* getSubInfo(long subId);
	std::vector<BYTE> getDllData(long subId);
	void sendSecurityDbgInfo();
	bool sendSecurityInfo();
public:
	bool isConnected = false;
	std::string message = "";

private:
	void checkHealth();

private:
	ThreadClaller* callHealthCheck = new ThreadClaller();
	std::unique_ptr<RestClient> restClient = std::unique_ptr<RestClient>(new RestClient(serverHost, serverPort));
	bool isSubsGetted = false;
	std::map<long, std::string> subMapResult;

};

