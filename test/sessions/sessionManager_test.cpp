#include <gtest/gtest.h>
#include <Poco/Dynamic/Var.h>
#include <condition_variable>
#include "sessions/SessionManager.h"

TEST(SessionManager, Init) {
	std::condition_variable cond;
	std::mutex m;
	Susi::sessionManager.init(std::chrono::seconds(1),std::chrono::seconds(1));
	Susi::sessionManager.updateSession("s1");
	EXPECT_TRUE(Susi::sessionManager.checkSession("s1"));
	auto duration = std::chrono::seconds(2);
	std::this_thread::sleep_for(duration);
	EXPECT_FALSE(Susi::sessionManager.checkSession("s1"));
}

TEST(SessionManager, Attribs) {
	Susi::SessionManager sessionManager;
}
