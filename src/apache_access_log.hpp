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
	bool processFile(const std::string& fileName, const std::string& startDate, const std::string& endDate);
	void setCountry(const std::string& country) {_country = country;}
	std::string getSummaryByCountry() const;
	std::string getSummaryByCountryHtml() const;
	std::string getItemsHtml() const;
private:
	bool processStream(std::istream& in, const std::string& startDate, const std::string& endDate);
	bool queryCountries();
	bool queryIspNames();

	static std::vector<std::string> tokenizeLogEntry(const std::string& line);
	static std::string extractDateFromDateTime(const std::string& dateTime);
	static std::string extractHourMinuteFromDateTime(const std::string& dateTime);

	AccessLogList _accessLogList;
	HttpRequestManager _httpRequestManager;
	std::string startDate;
	std::string endDate;
	std::string _country;
};
