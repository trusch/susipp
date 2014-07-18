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

#ifndef __SESSIONEVENTINTERFACE__
#define __SESSIONEVENTINTERFACE__

#include "events/EventSystem.h"
#include "sessions/SessionManager.h"

namespace Susi {

	void initSessionManagerEventInterface();

	void handleSessionAddAtribute(Event & event);
	void handleSessionSetAtribute(Event & event);
	void handleUpdateSession(Event & event);
	void handleCheckSession(Event & event);
}

#endif //__SESSIONEVENTINTERFACE__
