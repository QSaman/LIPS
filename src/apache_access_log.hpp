#pragma once

#include <string>
#include <vector>

#include "ip_address.hpp"

struct ApacheAccessLogEntry
{
	std::string datetimeStr;
	std::string userAgent;
	IPAddressQuery ipQuery;
};

class ApacheAccessLog
{
public:
	using AccessLogList = std::vector<ApacheAccessLogEntry>;
	void processFile(const std::string& fileName, const std::string& startDate, const std::string& endDate);
private:
	AccessLogList _accessLogList;
};
