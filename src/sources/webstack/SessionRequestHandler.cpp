#include "webstack/SessionRequestHandler.h"

void Susi::SessionRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request,
                       Poco::Net::HTTPServerResponse& response){
	Poco::Util::Application& app = Poco::Util::Application::instance();
	std::string id = "";
	try{
		Poco::Net::NameValueCollection cookies;
		request.getCookies(cookies);
		id = cookies["susisession"];
		if(!sessionManager.checkSession(id)){
			app.logger().debug("No valid session");
			Poco::Timestamp now;
			id = std::to_string(now.epochMicroseconds());
			sessionManager.updateSession(id);
			response.addCookie(Poco::Net::HTTPCookie{"susisession",id});
		}
	}catch(const std::exception & e){
		app.logger().debug("No session");
		Poco::Timestamp now;
		id = std::to_string(now.epochMicroseconds());
		sessionManager.updateSession(id);
		auto cookie = Poco::Net::HTTPCookie{"susisession",id};
		cookie.setPath("/");
		response.addCookie(cookie);
	}
	app.logger().debug("Session ID: "+id);
	Poco::Timestamp start;
	try{
		defaultHandler->handleRequest(request,response);
	}catch(const std::exception & e){
		Poco::Timestamp end;
		std::cout<<"error in http handler: "<<e.what()<<std::endl;
		std::cout<<"duration: "<<end-start<<std::endl;
	}
}