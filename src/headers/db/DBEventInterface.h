#ifndef __DBEVENTINTERFACE__
#define __DBEVENTINTERFACE__

#include "db/Database.h"
#include "events/EventSystem.h"


namespace Susi{

void handleQuery(Susi::Event & event);

void initDBEventInterface();
	
}


#endif // __DBEVENTINTERFACE__