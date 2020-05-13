#define private public
#include <apache_access_log.hpp>
#undef private

#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

const std::string accessLogEntry1 = "1.2.3.4 - - [25/May/2020:12:44:27 -0400] \"GET / HTTP/1.1\" 301 231 \"https://foo.com/\" \"Mozilla/5.0 (Linux; ) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome; en_US; 212676898)\"";

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
	tokens = ApacheAccessLog::tokenizeLogEntry(accessLogEntry1);
	ASSERT_TRUE(tokens.size() == 9);
	auto dateTime = tokens[ApacheAccessLog::DateTimeIndex];
	ASSERT_TRUE(dateTime == "[25/May/2020:12:44:27 -0400]");
	ASSERT_TRUE(ApacheAccessLog::extractDateFromDateTime(dateTime) == "25/May/2020");
	ASSERT_TRUE(ApacheAccessLog::extractHourMinuteFromDateTime(dateTime) == "12:44");
}

TEST(ApacheAccessLog, processStreamUnorderedSetTest)
{
	std::stringstream ss;
	ss << accessLogEntry1 << std::endl;
	ss << accessLogEntry1.substr(0, 25) + "12:44:59" + accessLogEntry1.substr(34) << std::endl;
	ss << accessLogEntry1.substr(0, 25) + "12:45:00" + accessLogEntry1.substr(34) << std::endl;
	ss << accessLogEntry1.substr(0, 25) + "12:45:59" + accessLogEntry1.substr(34) << std::endl;
	ss << accessLogEntry1.substr(0, 25) + "12:46:00" + accessLogEntry1.substr(34) << std::endl;	//TODO Edge case: compare time with previous one
	ApacheAccessLog accessLog;
	accessLog.processStream(ss, "2020-05-25", "2020-05-25");
	ASSERT_TRUE(accessLog._accessLogList.size() == 3);
}

TEST(ApacheAccessLog, processStreamDateIntervalTest)
{
	std::stringstream ss;
	ss << accessLogEntry1.substr(0, 13) + "24/May/2020" + accessLogEntry1.substr(24) << std::endl;
	ss << accessLogEntry1 << std::endl;
	ss << accessLogEntry1.substr(0, 25) + "12:44:59" + accessLogEntry1.substr(34) << std::endl;
	ss << accessLogEntry1.substr(0, 25) + "12:45:00" + accessLogEntry1.substr(34) << std::endl;
	ss << accessLogEntry1.substr(0, 13) + "26/May/2020" + accessLogEntry1.substr(24) << std::endl;
	ApacheAccessLog accessLog;
	accessLog.processStream(ss, "2020-05-25", "2020-05-25");
	ASSERT_TRUE(accessLog._accessLogList.size() == 2);
}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
