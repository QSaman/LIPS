#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>

#include <apache_access_log.hpp>

TEST(ApacheAccessLog, Exceptions)
{
	ApacheAccessLog accesLog;
	ASSERT_THROW(accesLog.processFile("invalid_file_name", "2020-12-01", "2020-12-02"), std::ios_base::failure);
}

TEST(ApacheAccessLog, tokenizeLogEntry)
{
	const std::string line1 = "[a b]";
	auto tokens = ApacheAccessLog::tokenizeLogEntry(line1);
	ASSERT_TRUE(tokens.size() == 1);
	const std::string line2 = "1.2.3.4 - - [25/May/2020:12:44:27 -0400] \"GET / HTTP/1.1\" 301 231 \"https://foo.com/\" \"Mozilla/5.0 (Linux; ) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome; en_US; 212676898)\"";
	tokens = ApacheAccessLog::tokenizeLogEntry(line2);
	ASSERT_TRUE(tokens.size() == 9);
	auto dateTime = tokens[ApacheAccessLog::DateTimeIndex];
	ASSERT_TRUE(dateTime == "[25/May/2020:12:44:27 -0400]");
	ASSERT_TRUE(ApacheAccessLog::extractDateFromDateTime(dateTime) == "25/May/2020");
}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
