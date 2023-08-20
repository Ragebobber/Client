#pragma once
#include "loginRequest.h"
#include "loginResponse.h"
#include "subResponse.h"
#include "dllResponse.h"
#include "securityInfo.h"

struct DtoImpl {
	//Login
	std::shared_ptr<LoginRequest> loginReq = std::make_shared <LoginRequest> ();
	std::shared_ptr<LoginResponse> loginRes = std::make_shared <LoginResponse> ( );
	//Subs
	std::shared_ptr<SubResponse> subRes = std::make_shared <SubResponse> ( );

	//dlls
	std::shared_ptr<DllResponse> dllRes = std::make_shared <DllResponse> ( );

	//sec
	std::shared_ptr<SecurityInfo> secInfReq = std::make_shared <SecurityInfo> ( );

};
