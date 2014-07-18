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

#include "webstack/AssetsRequestHandler.h"

void Susi::AssetsRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request,
                       							Poco::Net::HTTPServerResponse& response) {
	try{
		auto & app = Poco::Util::Application::instance();
		app.logger().debug("Assets request from " + request.clientAddress().toString()+" "+request.getURI());
		std::string fileLocation = rootDirectory.path()+"/"+request.getURI().substr(8);
		app.logger().debug("Target file: "+fileLocation);
		Poco::File f(fileLocation);
		
		Poco::Timestamp dateTime    = f.getLastModified();
		Poco::File::FileSize length = f.getSize();
		response.set("Last-Modified", Poco::DateTimeFormatter::format(dateTime, Poco::DateTimeFormat::HTTP_FORMAT));
		response.setContentLength(static_cast<int>(length));
		//response.setContentType(mediaType);
		response.setChunkedTransferEncoding(false);

		Poco::FileInputStream istr(fileLocation);
		if (istr.good()){
			std::ostream& ostr = response.send();
			Poco::StreamCopier::copyStream(istr, ostr);
		}else{
			throw std::exception{};
		}
	}catch(const std::exception & e){
		response.setChunkedTransferEncoding(true);
		response.setContentType("text/html");
		response.setStatus(Poco::Net::HTTPServerResponse::HTTPStatus::HTTP_NOT_FOUND);
		std::ostream& ostr = response.send();
		ostr << "<html><head><title>SUSI</title></head> ";
		ostr << "<body><p style=\"text-align: center;font-size: 48px;\"></br></br></br>";
		ostr << "(404) Not found: "+std::string(e.what());
		ostr << "</p></body></html>";
	}
}