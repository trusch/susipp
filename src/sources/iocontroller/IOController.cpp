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

#include "iocontroller/IOController.h"

 //high level 
std::size_t Susi::IOController::writeFile(std::string filename,char* ptr, std::size_t len) {

	std::ofstream s((char*)filename.c_str());
	s<<ptr;
	s.close();

	return len;
}

std::size_t Susi::IOController::writeFile(std::string filename,std::string content) {
	return this->writeFile(filename, (char*)content.c_str(), content.size());
}

std::string Susi::IOController::readFile(std::string filename) {

	std::string result = "";
	char * buffer;
	int length = 0;
	std::ifstream s((char*)filename.c_str());
	

	if(s) {
		// get length of file:
    	s.seekg (0, s.end);
		length = s.tellg();
	    s.seekg (0, s.beg);

    	buffer = new char [length];
	}

	// read data as a block:
    s.read(buffer,length); 

    if (!s) std::cout << "error: only " << s.gcount() << " could be read";
    
    s.close();

    result += std::string(buffer,length);

    delete[] buffer;

	return result;
}

bool Susi::IOController::deleteFile(std::string filename) {
	if( remove( filename.c_str() ) != 0 )
		//throw std::runtime_error("error deleting file");
		return false; //fail
  	else
  		return true;
}
/*)
// low level
std::fstream Susi::IOController::open(std::string filename, std::string mode) {
	
	std::fstream s((char*)filename.c_str());

	return s;
}
*/