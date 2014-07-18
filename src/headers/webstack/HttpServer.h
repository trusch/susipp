/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 * 
 * @author: Tino Rusch (tino.rusch@webvariants.de)
 */

#ifndef __HTTPSERVER__
#define __HTTPSERVER__

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include <iostream>

#include "webstack/RequestHandlerFactory.h"

namespace Susi {

class HttpServer {
protected:
	Poco::Net::ServerSocket serverSocket;
	Poco::Net::HTTPServer server;
public:
	HttpServer(unsigned short int port) : 
		serverSocket(port), 
		server(new RequestHandlerFactory,serverSocket,new Poco::Net::HTTPServerParams)
		{
			server.start();
			std::cout<<"started HTTP server on port "<<port<<std::endl;
		}
	HttpServer(unsigned short int port,Poco::Net::HTTPServerParams *params) : 
		serverSocket(port), 
		server(new RequestHandlerFactory,serverSocket,params)
		{
			server.start();
			std::cout<<"started HTTP server on port "<<port<<std::endl;
		}
	void stop(){
		server.stop();
	}
};

}

#endif // __HTTPSERVER__