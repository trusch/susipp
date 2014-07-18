#include "db/DBEventInterface.h"

void Susi::handleQuery(Susi::Event & event){
	try{
		std::string type = event.payload["type"];
		std::string uri = event.payload["uri"];
		std::string query = event.payload["query"];
		Susi::Database db(type,uri);
		auto res = db.query(query);
		Susi::Event resp(event.returnAddr,res);
		Susi::publish(resp);
	}catch(const std::exception & e){
		std::cerr<<"Error in handleQuery(): "<<e.what()<<std::endl;
	}
}

void Susi::initDBEventInterface(){
	subscribe("db::query",handleQuery);
}