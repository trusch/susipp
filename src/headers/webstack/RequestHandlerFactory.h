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

#ifndef __REQUESTHANDLERFACTORY__
#define __REQUESTHANDLERFACTORY__

#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Util/Application.h"
#include "webstack/NotFoundRequestHandler.h"
#include "webstack/AssetsRequestHandler.h"
#include "webstack/SessionRequestHandler.h"
#include "webstack/RedirectRequestHandler.h"
#include "webstack/WebSocketRequestHandler.h"

namespace Susi {


class RequestHandlerFactory: public Poco::Net::HTTPRequestHandlerFactory
{
public:
   Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request){
        //std::cout<<"got request with URI: "<<request.getURI()<<std::endl;
        if(request.getURI().find("/assets/")==0){
        	Poco::Util::Application& app = Poco::Util::Application::instance();
			std::string assetRoot = app.config().getString("webstack.assets");
        	return new SessionRequestHandler(new AssetsRequestHandler(assetRoot));
        }else if(request.getURI() == "/ws"){
        	return new SessionRequestHandler(new WebSocketRequestHandler());
        }else if(request.getURI() == "/"){
        	return new SessionRequestHandler(new RedirectRequestHandler());
        }
        return new SessionRequestHandler(new NotFoundRequestHandler());
    }
};

}

#endif // __REQUESTHANDLERFACTORY__