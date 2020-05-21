#pragma once

#include <string>
#include <vector>
#include <boost/date_time/gregorian/gregorian.hpp> 

#include "ip_address.hpp"
#include <web_service/http_request_manager.hpp>

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
		UserIndex = 2,
		DateTimeIndex = 3,
		RefererIndex = 7,
		UserAgentIndex = 8
	};

	using AccessLogList = std::vector<ApacheAccessLogEntry>;
	using UserList = std::vector<std::string>;
	using size_type = AccessLogList::size_type;

	bool processFile(const std::string& fileName, const boost::gregorian::date& startDate, const boost::gregorian::date& endDate);
	void setCountry(const std::string& country) {_country = country;}
	const std::string& country() const {return _country;}
	void setExcludedUsers(const UserList& excludedUsers) {_excludedUsers = excludedUsers;}

	std::string getSummaryByCountry() const;
	std::string getSummaryByCountryHtml() const;
	std::string getItemsHtml() const;
	size_type size() const {return _accessLogList.size();}
private:
	bool processStream(std::istream& in, const boost::gregorian::date& startDate, const boost::gregorian::date& endDate);
	bool populateIPFields();
	bool queryCountries();
	bool queryIspNames();

	static std::vector<std::string> tokenizeLogEntry(const std::string& line);
	static std::string extractDateFromDateTime(const std::string& dateTime);
	static std::string extractHourMinuteFromDateTime(const std::string& dateTime);

	AccessLogList _accessLogList;
	HttpRequestManager _httpRequestManager;
	std::string _country;
	UserList _excludedUsers;
};
