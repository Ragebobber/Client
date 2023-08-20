#pragma once


//extern std::shared_ptr<HttpStruct> httpStruct ;
//extern std::unique_ptr<RestClient> restClient ;
//extern std::unique_ptr<DtoImpl> dto;
//extern std::unique_ptr<ApiConnector> apiConnector;
//extern std::unique_ptr<Security> security;
//extern std::unique_ptr<Injection> injection;

extern HttpStruct* httpStruct;
extern NtFunctions* nt;
extern RestClient* restClient;
extern DtoImpl* dto;
extern ApiConnector* apiConnector;
extern Security* security;
extern Injection* injection;