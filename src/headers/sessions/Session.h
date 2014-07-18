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

#include <map>
#include <Poco/Timestamp.h>
#include <Poco/Dynamic/Var.h>
#include <chrono>

namespace Susi {
	class Session {
	protected:
		Poco::Timestamp deadline;
		std::map<std::string,Poco::Dynamic::Var> attributes;

	public:
		Session() : Session(std::chrono::seconds(10)) {}
		Session(std::chrono::seconds seconds);
		bool isDead();
		void addTime(std::chrono::seconds seconds);
		bool setAttribute(std::string key, Poco::Dynamic::Var value);
		Poco::Dynamic::Var getAttribute(std::string key);
		template <class T>
		T getAttribute(std::string key) {
			return attributes[key].convert<T>();
		}
		bool die();
	};
}
