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

#include "sessions/Session.h"
#include <iostream>
namespace Susi {
	Session::Session(std::chrono::seconds seconds) {
		this->addTime(seconds);
	}
	bool Session::isDead() {
		Poco::Timestamp now;
		//std::cout<<this->deadline.epochTime()<<" : "<<now.epochTime()<<std::endl;
		if(this->deadline <= now) return true;
		return false;
	}
	void Session::addTime(std::chrono::seconds seconds) {
		this->deadline += std::chrono::duration_cast<std::chrono::microseconds>(seconds).count();
	}
	// Session::getAttribute<T>() defined in header
	Poco::Dynamic::Var Session::getAttribute(std::string key) {
		return attributes[key];
	}
	/*bool Session::addAttribute(std::string key, Poco::Dynamic::Var value) {
		if(key.length() > 0) {
			this->attributes[key] = value;
			return true;
		}
		return false;
	}*/
	bool Session::setAttribute(std::string key, Poco::Dynamic::Var value) {
		this->attributes[key] = value;
		return true;
	}
	bool Session::die() {
		this->deadline = Poco::Timestamp();
		return true;
	}
}
