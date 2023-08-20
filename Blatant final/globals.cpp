#include "SDK.h"

//
//std::shared_ptr<HttpStruct> httpStruct(new HttpStruct );
//std::unique_ptr<RestClient> restClient(new RestClient );
//std::unique_ptr<DtoImpl> dto ( new DtoImpl );
//std::unique_ptr<ApiConnector> apiConnector ( new ApiConnector );
//std::unique_ptr<Security> security ( new Security );
//std::unique_ptr<Injection> injection ( new Injection );

 HttpStruct* httpStruct = new HttpStruct();
 NtFunctions* nt = new NtFunctions ( );
 RestClient* restClient = new RestClient ( );
 DtoImpl* dto = new DtoImpl ( );
 ApiConnector* apiConnector = new ApiConnector ( );
 Security* security = new Security ( );
 Injection* injection = new Injection ( );