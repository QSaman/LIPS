#pragma once

#include <string>
#include <vector>

#include "ip_address.hpp"

struct ApacheAccessLogEntry
{
	IPAddressInfo ipInfo;
	std::string datetimeStr;
	std::string referer;
	std::string userAgent;
};

class ApacheAccessLog
{
public:
	enum Fields
	{
		IpIndex = 0,
		DateTimeIndex = 3,
		RefererIndex = 7,
		UserAgentIndex = 8
	};
	using AccessLogList = std::vector<ApacheAccessLogEntry>;
	void processFile(const std::string& fileName, const std::string& startDate, const std::string& endDate);
private:
	void processStream(std::istream& in, const std::string& startDate, const std::string& endDate);
	static std::vector<std::string> tokenizeLogEntry(const std::string& line);
	static std::string extractDateFromDateTime(const std::string& dateTime);
	static std::string extractHourMinuteFromDateTime(const std::string& dateTime);

	AccessLogList _accessLogList;
};
