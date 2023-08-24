#include "SDK.h"

std::shared_ptr<HttpStruct> httpStruct(new HttpStruct);
std::shared_ptr<NtFunctions> nt(new NtFunctions);
std::unique_ptr<DtoImpl> dto(new DtoImpl);
std::unique_ptr<ApiConnector> apiConnector(new ApiConnector);
std::unique_ptr<Security> security(new Security);
std::unique_ptr<Injection> injection(new Injection);
