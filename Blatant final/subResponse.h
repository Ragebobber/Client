#pragma once
struct Product {
    long id;
    std::string name;
    std::string description;
    std::string status;
    std::string exeName;
};


struct Sub {
    long id;
    std::string expirationDate;
    bool active;
    Product* product = new Product ( );
};

struct SubResponse {
    std::vector<Sub*> userSubs;

	bool convertSubsFromJSON (HttpStruct* response ) {
		if ( !response->error.empty ( ) || response->body.empty ( ) )
			return false;
		json::value jv = json::parse ( response->body );
        if ( jv.is_array ( ) ) {
            for ( auto& value : jv.as_array ( ) ) {
                Sub* currentSub = new Sub ( );
                currentSub->id = json::value_to<long> ( value.at ( S_DTO_ID ) );
                currentSub->expirationDate = json::value_to< std::string> ( value.at ( S_DTO_EX_DATE ) );
                currentSub->active = json::value_to<bool> ( value.at ( S_DTO_ACTIVE ) );
                if ( value.at ( S_DTO_PRODUCT_ID ).is_object ( ) ) {
                    currentSub->product->id = json::value_to<long> ( value.at ( S_DTO_PRODUCT_ID ).at ( S_DTO_ID ) );
                    currentSub->product->name = json::value_to< std::string> ( value.at ( S_DTO_PRODUCT_ID ).at ( S_DTO_NAME ) );
                    currentSub->product->description = json::value_to< std::string> ( value.at ( S_DTO_PRODUCT_ID ).at ( S_DTO_DESCR ) );
                    currentSub->product->status = json::value_to< std::string> ( value.at ( S_DTO_PRODUCT_ID ).at ( S_DTO_STATUS ) );
                    currentSub->product->exeName = json::value_to< std::string> ( value.at ( S_DTO_PRODUCT_ID ).at ( S_DTO_ALT_ANAME ) );

                }
                userSubs.push_back ( currentSub );
            }
            return true;
        }
        return false;
	}
};