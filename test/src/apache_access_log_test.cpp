#define private public
#define protected public
#include <geoip_webservice.hpp>
#include <apache_access_log.hpp>
#undef protected
#undef private

#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>

const std::string accessLogEntry1 = "8.8.8.8 - - [25/May/2020:12:44:27 -0400] \"GET / HTTP/1.1\" 301 231 \"https://foo.com/\" \"Mozilla/5.0 (Linux; ) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome; en_US; 212676898)\"";
const std::string accessLogEntry2 = "217.218.78.26 - - [25/May/2020:13:44:27 -0400] \"GET / HTTP/1.1\" 301 231 \"https://bar.com/\" \"Mozilla/5.0 (Linux; ) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome; en_US; 212676898)\"";
const std::string accessLogEntry3 = "193.8.139.22 - - [25/May/2020:14:44:27 -0400] \"GET / HTTP/1.1\" 301 231 \"https://baz.com/\" \"Mozilla/5.0 (Linux; ) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome; en_US; 212676898)\"";

std::vector<std::string> accessLogEntries = {accessLogEntry1, accessLogEntry2, accessLogEntry3};

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

TEST(ApacheAccessLog, TestQueryCountries)
{
	std::stringstream ss;

	std::for_each(accessLogEntries.begin(), accessLogEntries.end(),
	              [&ss](const std::string& entry)
				  {
				  	ss << entry << std::endl;
				  });
	ApacheAccessLog accessLog;
	accessLog.processStream(ss, "2020-05-25", "2020-05-25");
	ASSERT_TRUE(accessLog._accessLogList.size() == 3);
	auto checkLogFields = [](const ApacheAccessLogEntry& entry)
	{
		ASSERT_FALSE(entry.datetimeStr.empty());
		ASSERT_FALSE(entry.referer.empty());
		ASSERT_FALSE(entry.userAgent.empty());
	};

	std::for_each(accessLog._accessLogList.begin(), accessLog._accessLogList.end(),
	              [&checkLogFields](const ApacheAccessLogEntry& entry)
				  {
				  	checkLogFields(entry);
					ASSERT_TRUE(entry.ipInfo.country.empty());
				  });

	accessLog.queryCountries();
	std::for_each(accessLog._accessLogList.begin(), accessLog._accessLogList.end(),
	              [&checkLogFields](const ApacheAccessLogEntry& entry)
				  {
				  	checkLogFields(entry);
					ASSERT_FALSE(entry.ipInfo.country.empty());
				  });

	for (auto& entry : accessLog._accessLogList)
	{
		HttpSession::_cache->erase(accessLog._httpRequestManager._countryWS[0]->_httpSession.generateUrl(entry.ipInfo.ipAddress));
	}

	for (auto& val : accessLog._httpRequestManager._countryWS)
		val->resetTimer();
}

TEST(ApacheAccessLog, TestqueryIspNames)
{
	std::stringstream ss;
	for (auto& val : accessLogEntries)
		ss << val << std::endl;

	ApacheAccessLog accessLog;
	accessLog.processStream(ss, "2020-05-25", "2020-05-25");
	ASSERT_TRUE(accessLog._accessLogList.size() == 3);
	auto checkLogFields = [](const ApacheAccessLogEntry& entry)
	{
		ASSERT_FALSE(entry.datetimeStr.empty());
		ASSERT_FALSE(entry.referer.empty());
		ASSERT_FALSE(entry.userAgent.empty());
	};

	for (auto& entry : accessLog._accessLogList)
	{
		checkLogFields(entry);
		ASSERT_TRUE(entry.ipInfo.ispName.empty());
	}

	accessLog.queryIspNames();

	for (auto& entry : accessLog._accessLogList)
	{
		checkLogFields(entry);
		ASSERT_FALSE(entry.ipInfo.ispName.empty());
	}

	for (auto& entry : accessLog._accessLogList)
	{
		HttpSession::_cache->erase(accessLog._httpRequestManager._countryWS[0]->_httpSession.generateUrl(entry.ipInfo.ipAddress));
	}

	for (auto& val : accessLog._httpRequestManager._countryWS)
		val->resetTimer();
	for (auto& val : accessLog._httpRequestManager._ispNameWS)
		val->resetTimer();
}
