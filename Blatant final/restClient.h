#pragma once

#define serverHost "localhost"
#define serverPort "8080"

// Need create TCP correctly not hardcoded
class RestClient {
public:
	RestClient ( );
	~RestClient ( );
	HttpStruct*	 getRequest ( std::string endPoint );
	HttpStruct*  postRequest ( std::string endPoint, std::string jsonBody );
private:
	
	
};

