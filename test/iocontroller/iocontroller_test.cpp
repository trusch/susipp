#include "gtest/gtest.h"
#include "iocontroller/IOController.h"
#include "util/TestHelper.h"

TEST(IOController,WriteFile){
	Susi::IOController controller;
	std::string data = "foobar";
	std::size_t bs = controller.writeFile("./foobar.dat",data);
	EXPECT_EQ(6,bs);

	std::size_t w_size = 1024*1024*5;
	data = Susi::Util::__createRandomString(w_size);
	bs   = controller.writeFile("./foobar.dat",data);

	EXPECT_EQ(w_size,bs);

}

TEST(IOController,ReadFile){
	Susi::IOController controller;

	// create File
	std::string data = "foobar";	
	controller.writeFile("./foobar.dat",data);

	std::string result = controller.readFile("./foobar.dat");
	EXPECT_EQ(data,result);
}

TEST(IOController,DeleteFile){
	Susi::IOController controller;
	// create File
	std::string data = "foobar";
	controller.writeFile("./foobar.dat",data);

	// delete first time
	bool result = false;
	result = controller.deleteFile("./foobar.dat");
	EXPECT_TRUE(result);

	// delete secound time
	result = true;
	result = controller.deleteFile("./foobar.dat");
	EXPECT_FALSE(result);
}