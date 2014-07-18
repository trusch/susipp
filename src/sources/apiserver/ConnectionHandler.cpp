#include "apiserver/ConnectionHandler.h"

void Susi::ConnectionHandler::handleConnection(std::shared_ptr<Susi::Connection> conn){
	/*Poco::Dynamic::Struct<std::string> payload(std::map<std::string,Poco::Dynamic::Var>{
		std::make_pair("id",conn->getSessionID())
	});*/
	auto payload = Susi::Event::Payload({
		{"id",conn->getSessionID()}
	}); 
	auto resp = Susi::request("session::check",payload,std::chrono::milliseconds(250));
	if(conn->getSessionID() == "" || !resp.convert<bool>()){
		Poco::Timestamp now;
		conn->setSessionID(std::to_string(now.epochMicroseconds()));
		Poco::Dynamic::Struct<std::string> payload(std::map<std::string,Poco::Dynamic::Var>{
			std::make_pair("id",conn->getSessionID())
		});
		Susi::Event event("session::update",payload);
		Susi::publish(event);
	}
	Poco::Dynamic::Var packet;
	std::cout<<"got new connection with sessionID: "<<conn->getSessionID()<<std::endl;
	while(true){
		if(conn->recv(packet)){
			Susi::ConnectionHandler::handlePacket(conn,packet);
		}else{
			break;
		}	
	}
	std::cout<<"lost connection with sessionID: "<<conn->getSessionID()<<std::endl;
}

void Susi::ConnectionHandler::sendStatus(std::shared_ptr<Susi::Connection> conn, Poco::Dynamic::Var & request, std::string msg, bool success){
	using Susi::Util::getFromVar;
	try{
		std::cout<<"send status!"<<std::endl;
		auto payload = Susi::Event::Payload({
			{"type","status"},
			{"key",success ? "ok" : "error"}
		});
		if(!getFromVar(request,"id").isEmpty()){
			payload["id"] = getFromVar(request,"id");
		}
		if(msg!=""){
			payload["payload"] = msg;
		}
		std::cout<<"send"<<std::endl;
		Poco::Dynamic::Var v(payload);
		conn->send(v);
	}catch(const std::exception & e){
		std::cerr<<e.what()<<std::endl;
	}
}

void Susi::ConnectionHandler::handlePacket(
		std::shared_ptr<Susi::Connection> conn, 
		Poco::Dynamic::Var packet){
	using Susi::Util::getFromVar;
	try{
		auto type = getFromVar(packet,"type");
		auto key = getFromVar(packet,"key");
		if(type.isEmpty() || key.isEmpty()){
			Susi::ConnectionHandler::sendStatus(conn,packet,std::string("no type or no key supplied."),false);	
		}else{
			if(type == "publish"){
				Susi::ConnectionHandler::handlePublish(conn,packet);				
			}else if(type == "subscribe"){
				Susi::ConnectionHandler::handleSubscribe(conn,packet);
			}else if(type == "unsubscribe"){
				Susi::ConnectionHandler::handleUnsubscribe(conn,packet);
			}
		}
	}catch(const std::exception & e){
		Susi::ConnectionHandler::sendStatus(conn,packet,e.what(),false);
	}
}

void Susi::ConnectionHandler::handlePublish(std::shared_ptr<Susi::Connection> conn, Poco::Dynamic::Var packet){
	using Susi::Util::getFromVar;
	auto key = getFromVar(packet,"key");
	auto payload = getFromVar(packet,"payload");
	auto returnAddr = getFromVar(packet,"returnaddr");
	Susi::Event event(key.toString());
	event.sessionID = conn->getSessionID();
	if(!payload.isEmpty()){
		event.payload = payload;
	}
	if(!returnAddr.isEmpty()){
		event.returnAddr = returnAddr.toString();
	}
	Susi::publish(event);
	Susi::ConnectionHandler::sendStatus(conn,packet,std::string("successfully published to "+key.toString()),true);
}

void Susi::ConnectionHandler::handleSubscribe(std::shared_ptr<Susi::Connection> conn, Poco::Dynamic::Var packet){
	using Susi::Util::getFromVar;
	auto keyVar = getFromVar(packet,"key");
	auto idVar = getFromVar(packet,"id");
	std::string topic = keyVar.convert<std::string>();
	int id = 0;
	if(idVar.isInteger()){
		id = idVar.extract<int>();
	}
	conn->subscribe(topic,id);
}

void Susi::ConnectionHandler::handleUnsubscribe(std::shared_ptr<Susi::Connection> conn, Poco::Dynamic::Var packet){
	using Susi::Util::getFromVar;
	auto keyVar = getFromVar(packet,"key");
	auto idVar = getFromVar(packet,"id");
	std::string topic = keyVar.convert<std::string>();
	int id = 0;
	if(idVar.isInteger()){
		id = idVar.convert<int>();
	}
	conn->unsubscribe(topic,id);
}
