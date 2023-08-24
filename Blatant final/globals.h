#pragma once

extern std::shared_ptr<HttpStruct> httpStruct;
extern std::shared_ptr<NtFunctions> nt;
extern std::unique_ptr<DtoImpl> dto;
extern std::unique_ptr<ApiConnector> apiConnector;
extern std::unique_ptr<Security> security;
extern std::unique_ptr<Injection> injection;
