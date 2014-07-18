/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 * 
 * @author: Thomas Krause (thomas.krause@webvariants.de)
 */

#ifndef __IO_CONTROLLER__
#define __IO_CONTROLLER__

#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>


namespace Susi {

class IOController {
	public:
		// high level
		std::size_t writeFile(std::string filename,char* ptr, std::size_t len);
 		std::size_t writeFile(std::string filename,std::string content);
		std::string readFile(std::string filename);
		bool deleteFile(std::string filename);

		// low level
		//std::fstream open(std::string filename, std::string mode);
};


}

#endif // __IO_CONTROLLER__