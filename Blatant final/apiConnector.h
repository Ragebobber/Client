#pragma once

class ApiConnector {
public:
	ApiConnector ( );
	~ApiConnector ( );

	bool login ( );
	std::map<long, std::string> getUserSubs ( );
	Sub* getSubInfo ( long subId );
	std::vector<BYTE> getDllData ( long subId );
	void sendSecurityDbgInfo ( );
	bool sendSecurityInfo ( );
public:
	bool isConnected = false;
	std::string message = "";

private:
	void checkHealth ( );

private:
	ThreadClaller* callHealthCheck = new ThreadClaller ( );
	bool isSubsGetted = false;
	std::map<long, std::string> subMapResult;

};

