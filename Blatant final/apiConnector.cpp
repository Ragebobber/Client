#include "SDK.h"

ApiConnector::ApiConnector ( ) {
	callHealthCheck->start ( 5000, [&]( ) { checkHealth ( );} );
}

ApiConnector::~ApiConnector ( ) {

}

bool ApiConnector::login ( ) {
	if ( !isConnected )
		return false;

	auto loginData = dto->loginReq;

	if ( !loginData || 
		loginData->login.empty ( ) || 
		loginData->password.empty ( ) || 
		loginData->hwid.empty ( ) )
		return false;
	
	httpStruct->error.clear ( );
	message.clear ( );

	auto authResponse = restClient->postRequest ( S_API_LOGIN_ENDPOINT, loginData->convertToAuthJSON() );
	auto isLogin = dto->loginRes->convertFromJSOM ( authResponse );
	if ( isLogin ) {
		httpStruct->error.clear ( );
		message.clear ( );
		return true;
	}
	return false;
}

std::map<long, std::string> ApiConnector::getUserSubs ( ) {
	
	if ( isSubsGetted ) {
		return subMapResult;
	}

	if ( !isConnected )
		return subMapResult;

	httpStruct->error.clear ( );
	message.clear ( );

	auto userSubResponse = restClient->getRequest ( S_API_SUB_ENDPOINT );

	isSubsGetted = dto->subRes->convertSubsFromJSON ( userSubResponse );

	if ( isSubsGetted ) {
		for ( auto sub : dto->subRes->userSubs ) {
			subMapResult[sub->id] = sub->product->name;
		}
	}
	return subMapResult;
}

void ApiConnector::checkHealth ( ) {
	HttpStruct* checkHealthResponse = restClient->getRequest ( S_API_HEALTH_ENDPOINT );
	if ( checkHealthResponse && !checkHealthResponse->body.empty() && json::parse ( checkHealthResponse->body ) == S_API_HEALTH_RESPONSE ) {
		isConnected = true;
		return;
	}
	isConnected = false;
}

Sub* ApiConnector::getSubInfo ( long subId ) {
	if ( subId == -1 )
		return nullptr;

	for ( auto elem : dto->subRes->userSubs ) {
		if ( elem->id == subId )
			return elem;
	}

	return nullptr;
}


std::vector<BYTE> ApiConnector::getDllData ( long subId ) {	
	if ( !isConnected )
		return {};
	auto response = restClient->getRequest ( S_API_LOAD_SUB + std::to_string ( subId ) );

	return dto->dllRes->convertFromJSOM ( response );
}

void ApiConnector::sendSecurityDbgInfo ( ) {
	if ( !isConnected )
		return;

	auto secResponse = restClient->postRequest ( S_API_SECURITY_DBG_INFO, S_EMPTY_STR );
}

bool ApiConnector::sendSecurityInfo (  ) {
	if ( !isConnected )
		return false;

	auto secResponse = restClient->postRequest ( S_API_SECURITY_INFO,  dto->secInfReq->convertToAuthJSON());

	if ( !secResponse->body.empty ( ) &&  secResponse->body == "0x13" )
		return true;

	return false;

}