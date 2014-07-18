#ifndef __DATABASE__
#define __DATABASE__

#include <soci.h>
#include <sqlite3/soci-sqlite3.h>
#include <firebird/soci-firebird.h>
#include <Poco/Dynamic/Struct.h>
#include <iostream>

namespace Susi {

class Database{
protected:
	soci::session session;
public:
	Database(std::string dbtype,std::string connectURI) : session(dbtype,connectURI) {};
	Poco::Dynamic::Var query(std::string query);
};

}

#endif // __DATABASE__