#pragma once

#include <string>
#include <vector>

#include "ip_address.hpp"
#include "http_request_manager.hpp"

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
	bool getSummaryByCountry(std::string& result);
	bool getSummaryByCountryHtml(std::string& result);
	bool getItemsHtml(const std::string& country, std::string& result);
private:
	void processStream(std::istream& in, const std::string& startDate, const std::string& endDate);
	bool queryCountries();
	bool queryIspNames();
	bool filterCountry(const std::string& country);

	static std::vector<std::string> tokenizeLogEntry(const std::string& line);
	static std::string extractDateFromDateTime(const std::string& dateTime);
	static std::string extractHourMinuteFromDateTime(const std::string& dateTime);

	AccessLogList _accessLogList;
	HttpRequestManager _httpRequestManager;
	std::string startDate;
	std::string endDate;
};
