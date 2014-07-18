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

#ifndef __SESSIONMANAGER__
#define __SESSIONMANAGER__

#include <map>
#include <Poco/Timestamp.h>
#include <mutex>
#include <chrono>
#include <thread>
#include <Poco/Util/Application.h>
#include <Poco/Dynamic/Var.h>
#include "sessions/Session.h"

namespace Susi{

class SessionManager {
protected:
	std::map<std::string, Session> sessions;
	std::mutex mutex;
	std::chrono::seconds stdLifetime;
	int checkSessions();
public:
	bool checkSession(std::string id);
	void updateSession(std::string id);
	bool setSessionAttribute(std::string sessionID, std::string key, Poco::Dynamic::Var value);
	Poco::Dynamic::Var getSessionAttribute(std::string sessionID, std::string key);
	template <class T>
	T getSessionAttribute(std::string sessionID, std::string key) {
		std::lock_guard<std::mutex> lock(mutex);
		if (sessionID.length() > 0 && sessions.count(sessionID) > 0)
		{
			return sessions[sessionID].getAttribute<T>(key);
		}
		return Poco::Dynamic::Var();
	}
	void init(std::chrono::seconds stdSessionLifetime, std::chrono::seconds checkInterval);
};

static Susi::SessionManager sessionManager;

}

#endif //__SESSIONMANAGER__
