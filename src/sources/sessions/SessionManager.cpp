/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @authors: Tino Rusch (tino.rusch@webvariants.de), Christian Sonderfeld (christian.sondefeld@webvariants.de)
 */

#include "sessions/SessionManager.h"


namespace Susi {
	void SessionManager::init(std::chrono::seconds stdSessionLifetime, std::chrono::seconds checkInterval) {
		if(stdLifetime.count() <= 0) {
			stdLifetime = std::chrono::seconds(1);
		}
		this->stdLifetime = stdSessionLifetime;
		std::thread t{[this,checkInterval](){
			while(true){
				try{
					this->checkSessions();
			    	std::this_thread::sleep_for(checkInterval);
				}catch(const std::exception & e){
				}
			}
		}};
		t.detach();
	}

	int SessionManager::checkSessions(){
		std::lock_guard<std::mutex> lock(mutex);
		std::map<std::string, Session> newSessions;
		int deleted = 0;
		for(auto & kv : sessions){
			if(!kv.second.isDead()){
				newSessions[kv.first] = kv.second;
			}else{
				deleted++;
			}
		}
		sessions = newSessions;
		return deleted;
	}

	bool SessionManager::checkSession(std::string id){
		std::lock_guard<std::mutex> lock(mutex);
		try{
			return !sessions.at(id).isDead();
		}catch(const std::exception & e){
			return false;
			//throw e;
		}
	}

	bool SessionManager::setSessionAttribute(std::string sessionID, std::string key, Poco::Dynamic::Var value) {
		std::lock_guard<std::mutex> lock(mutex);
		if (sessionID.length() > 0)
		{
			if(sessions.count(sessionID) > 0) {
				this->sessions[key].setAttribute(key, value);
			}
			else {
				Session session;
				session.setAttribute(key, value);
				sessions[sessionID] = session;
			}
			return true;
		}
		return false;
	}

	// getSessionAttribute<T>() defined in header
	Poco::Dynamic::Var SessionManager::getSessionAttribute(std::string sessionID, std::string key) {
		std::lock_guard<std::mutex> lock(mutex);
		if (sessionID.length() > 0 && sessions.count(sessionID) > 0)
		{
			return sessions[sessionID].getAttribute(key);
		}
		return Poco::Dynamic::Var();
	}

	void SessionManager::updateSession(std::string id){
		std::lock_guard<std::mutex> lock(mutex);
		if(sessions.count(id) > 0 && !sessions[id].isDead()) {
			sessions[id].addTime(stdLifetime);
		}
		else {
			sessions[id] = Session(this->stdLifetime);
		}
	}
}
