#include <gtest/gtest.h>
#include <Poco/Dynamic/Var.h>
#include "sessions/Session.h"

TEST(Session, Init) {
	auto session = Susi::Session();
	EXPECT_FALSE(session.isDead());
}

TEST(Session, Attribs) {
	/**
	 * Extend :)
	 */
	auto session = Susi::Session();
	EXPECT_TRUE(session.getAttribute("super").isEmpty());
	session.setAttribute("real", "testResult");
	auto result = session.getAttribute<std::string>("real");
	EXPECT_EQ("testResult", result);
}

TEST(Session, Die) {
	auto session = Susi::Session();
	session.die();
	EXPECT_TRUE(session.isDead());
}
