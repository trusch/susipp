#include "apiserver/Connection.h"

bool Susi::TCPConnection::send(Poco::Dynamic::Var & data) {
	std::lock_guard<std::mutex> lock(mutex);
	std::string s = data.toString()+"\n";
	return socket.sendBytes(s.c_str(),s.size()) == (int)s.size();
}

bool Susi::TCPConnection::recv(Poco::Dynamic::Var & data) {
	int opening = 0;
	int closing = 0;
	std::string packet = "";
	while(!ss.eof()){
		std::string part = "";
		std::getline(ss,part,'}');
		if(part.size()==0)return false;
		part += "}";
		packet += part;
		opening += countInString(part,'{');
		closing += countInString(part,'}'); 
		if(opening==closing)break;
	}
	std::cout<<"packet: "<<packet<<std::endl;
	Poco::JSON::Parser parser;
	try{
		data = parser.parse(packet);
	}catch(const std::exception & e){
		std::cout<<e.what()<<std::endl;
		return false;
	}
	return true;
}

bool Susi::WebSocketConnection::send(Poco::Dynamic::Var & data){
	std::lock_guard<std::mutex> lock(mutex);
	std::string s = data.toString()+"\n";
	return socket.sendFrame((void*)s.c_str(),s.size()) == (int)s.size();
}

bool Susi::WebSocketConnection::recv(Poco::Dynamic::Var & data){
	int flags,bs;
	char buff[1024];
	if((bs = socket.receiveFrame(buff,1024,flags))>0){
		try{
			Poco::JSON::Parser parser;
			std::string s(buff,bs);
			data = parser.parse(s);
			return true;
		}catch(const std::exception & e){
			std::cerr<<e.what()<<std::endl;
		}
	}
	return false;
}

int Susi::TCPConnection::countInString(std::string s, char c){
	int count = 0;
	for(std::size_t i=0;i<s.size();i++){
		if(s[i]==c)count++;
	}
	return count;
}


void Susi::Connection::subscribe(std::string topic,int requestID){
	for(auto & kv : subscriptions){
		if(kv.second == topic){
			send(Susi::Event::Payload({
				{"type","status"},
				{"key","error"},
				{"payload","you are allready subscribed to "+topic},
				{"id",requestID}
			}));
			return;
		}
	}
	long id = Susi::subscribe(topic,[this,requestID](Susi::Event & event){
		auto msg = Susi::Event::Payload({
			{"type","event"},
			{"key",event.topic},
		});
		if(event.returnAddr != ""){
			msg["returnaddr"] = event.returnAddr;
		}
		if(!event.payload.isEmpty()){
			msg["payload"] = event.payload;
		}
		send(msg);
	});
	subscriptions.push_back({id,topic});
	send(Susi::Event::Payload({
		{"type","status"},
		{"key","ok"},
		{"payload","successfuly subscribed to "+topic},
		{"id",requestID}
	}));
}

void Susi::Connection::unsubscribe(std::string topic,int requestID){
	for(auto & kv : subscriptions){
		if(kv.second == topic){
			Susi::unsubscribe(topic,kv.first);
			send(Susi::Event::Payload({
				{"type","status"},
				{"key","ok"},
				{"payload","successfully unsubscribed from "+topic},
				{"id",requestID}
			}));
			return;
		}
	}
	send(Susi::Event::Payload({
		{"type","status"},
		{"key","error"},
		{"payload","you are not subscribed to "+topic},
		{"id",requestID}
	}));
}