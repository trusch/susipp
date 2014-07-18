/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Christian Sonderfeld (christian.sonderfeld@webvariants.de)
 */

#include <Poco/Dynamic/Var.h>

#include "sessions/SessionManagerEventInterface.h"

namespace Susi {
	void handleSessionGetAttribute(Event & event) {
		try{
			std::string sessionID = event.payload["id"];
			std::string key = event.payload["key"];
			answerEvent(event, sessionManager.getSessionAttribute(sessionID, key));
		}catch(const std::exception & e){
			std::cerr<<"Error in handleQuery(): "<<e.what()<<std::endl;
			answerEvent(event, false);
		}
	}
	void handleSessionSetAttribute(Event & event) {
		try{
			std::string sessionID = event.payload["id"];
			std::string key = event.payload["key"];
			Poco::Dynamic::Var value = event.payload["value"];
			answerEvent(event, sessionManager.setSessionAttribute(sessionID, key, value));
		}catch(const std::exception & e){
			std::cerr<<"Error in handleQuery(): "<<e.what()<<std::endl;
			answerEvent(event, false);
		}
	}
	void handleUpdateSession(Event & event) {
		try{
			std::string sessionID = event.payload["id"];
			sessionManager.updateSession(sessionID);
			answerEvent(event, true);
		}catch(const std::exception & e){
			std::cerr<<"Error in handleQuery(): "<<e.what()<<std::endl;
			answerEvent(event, false);
		}
	}
	void handleCheckSession(Event & event) {
		try{
			std::string sessionID = event.payload["id"];
			answerEvent(event, sessionManager.checkSession(sessionID));
		}catch(const std::exception & e){
			std::cerr<<"Error in handleQuery(): "<<e.what()<<std::endl;
			answerEvent(event, false);
		}
	}

	void initSessionManagerEventInterface() {
		subscribe("session::setAtribute", handleSessionSetAttribute);
		subscribe("session::getAtribute", handleSessionGetAttribute);
		subscribe("session::update", handleUpdateSession);
		subscribe("session::check", handleCheckSession);
	}
}
