#include "apache_access_log.hpp"

#include <boost/date_time/gregorian/gregorian.hpp> 
#include <boost/exception/exception.hpp>
#include <fstream>
#include <iostream>
#include <cctype>
#include <unordered_set>
#include <utility>
#include <algorithm>

namespace
{
	using CountryFrequency = std::pair<std::string, std::size_t>;
	using CountryFrequencyList = std::vector<CountryFrequency>;

	CountryFrequencyList constructCountryFreq(ApacheAccessLog::AccessLogList& accessLogList)
	{
		CountryFrequencyList ret;
		std::sort(accessLogList.begin(), accessLogList.end(), [](const ApacheAccessLogEntry& left, const ApacheAccessLogEntry& right)
				  {
				  	return left.ipInfo.country < right.ipInfo.country;
				  });
		std::size_t i, j;
		for (i = 0; i < accessLogList.size(); i = j)
		{
			for (j = i + 1; j < accessLogList.size() && accessLogList[i].ipInfo.country == accessLogList[j].ipInfo.country; ++j);
			CountryFrequency cf;
			cf.first = accessLogList[i].ipInfo.country;
			cf.second = j - i;
			ret.push_back(cf);
		}
		return ret;
	}

	const std::string& getHtmlHead()
	{
		const static std::string htmlHead = 
"<head>\n"
"<style>\n"
"table {\n"
  "font-family: arial, sans-serif;\n"
  "border-collapse: collapse;\n"
  "width: 100%;\n"
"}\n"
"\n"
"td, th {\n"
  "border: 1px solid #dddddd;\n"
  "text-align: left;\n"
  "padding: 8px;\n"
"}\n"
"\n"
"tr:nth-child(even) {\n"
  "background-color: #dddddd;\n"
"}\n"
"</style>\n"
"</head>\n";
		return htmlHead;
	}
}

std::vector<std::string> ApacheAccessLog::tokenizeLogEntry(const std::string& line)
{
	std::vector<std::string> tokens;

	std::string::size_type j = 0;
	for (std::string::size_type i = 0; i < line.size(); i = j + 1)
	{
		if (std::isspace(line[i]))
			continue;

		j = i + 1;
		bool quotes = line[i] == '"';
		int bracket = line[i] == '[' ? 1 : 0;
		for (; j < line.size() && (!std::isspace(line[j]) || quotes || bracket != 0); ++j)
		{
			if (line[j] == '"')
				quotes = (quotes != true);
			else if (line[j] == '[')
				++bracket;
			else if (line[j] == ']')
				--bracket;
		}
		tokens.push_back(line.substr(i, j - i));
	}
	return tokens;
}

std::string ApacheAccessLog::extractDateFromDateTime(const std::string& dateTime)
{
	auto colonIndex = dateTime.find(':');
	return dateTime.substr(1, colonIndex - 1);
}

std::string ApacheAccessLog::extractHourMinuteFromDateTime(const std::string& dateTime)
{
	auto colonIndex = dateTime.find(':');
	return dateTime.substr(colonIndex + 1, 5);
}

void ApacheAccessLog::processFile(const std::string& fileName, const std::string& startDate, const std::string& endDate)
{
	std::ifstream fin;
	fin.exceptions(std::ifstream::badbit | std::ifstream::failbit);
	fin.open(fileName);
	processStream(fin, startDate, endDate);
}

void ApacheAccessLog::processStream(std::istream& in, const std::string& startDate, const std::string& endDate)
{
	this->startDate = startDate;
	this->endDate = endDate;

	using namespace boost::gregorian;
	date start(from_simple_string(startDate));
	date end(from_simple_string(endDate));

	std::unordered_set<std::string> mark;

	std::string line;
	while (getline(in, line))
	{
		auto tokens = tokenizeLogEntry(line);
		if (tokens.size() <= 7 || !IPAddressInfo::isPublicIP(tokens[IpIndex]))
			continue;
		date current(from_uk_string(extractDateFromDateTime(tokens[DateTimeIndex])));
		if (current < start)
			continue;
		if (current > end)
			return;
		if (!mark.insert(tokens[IpIndex] + extractHourMinuteFromDateTime(tokens[DateTimeIndex])).second)
			continue;

		ApacheAccessLogEntry logEntry;
		logEntry.ipInfo.ipAddress = tokens[IpIndex];
		
		//TODO Check country is valid

		logEntry.datetimeStr = tokens[DateTimeIndex];
		logEntry.referer = tokens[RefererIndex];
		logEntry.userAgent = tokens[UserAgentIndex];
		_accessLogList.push_back(logEntry);
	}
}

bool ApacheAccessLog::queryCountries()
{
	for (auto& val : _accessLogList)
		if (!_httpRequestManager.getCountry(val.ipInfo))
			return false;
	return true;
}

bool ApacheAccessLog::queryIspNames()
{
	for (auto& val : _accessLogList)
		if (!_httpRequestManager.getISP(val.ipInfo))
			return false;
	return true;
}

bool ApacheAccessLog::getSummaryByCountry(std::string& result)
{
	result.clear();
	if (!queryCountries())
		return false;
	auto list = constructCountryFreq(_accessLogList);
	std::for_each(list.begin(), list.end(),
	              [&result](const CountryFrequency& cf)
				  {
				  	result += cf.first + ": " + std::to_string(cf.second) + "\n";
				  });
	return true;
}

bool ApacheAccessLog::getSummaryByCountryHtml(std::string& result)
{
	result.clear();
	if (!queryCountries())
		return false;

	result = "<!DOCTYPE html>\n<html>\n" + getHtmlHead();
	result += 
"<body>\n"
"<table>\n"
"  <tr>\n"
"    <th>Country</th>\n"
"    <th>Frequency</th>\n"
"  </tr>\n";
	auto list = constructCountryFreq(_accessLogList);
	std::for_each(list.begin(), list.end(), 
	              [&result](const CountryFrequency& cf)
				  {
				  	result += "  <tr>\n"
					          "    <td>" + cf.first + "</td>\n"
					          "    <td>" + std::to_string(cf.second) + "</td>\n"
					          "  </tr>\n";
				  });
	result += "</table>\n</body>\n</html>\n";
	return true;
}

bool ApacheAccessLog::filterCountry(const std::string& country)
{
	if (!queryCountries())
		return false;

	{
		AccessLogList tmp;
		std::for_each(_accessLogList.begin(), _accessLogList.end(),
					  [&tmp, &country](const ApacheAccessLogEntry& entry)
					  {
						if (entry.ipInfo.country == country)
							tmp.push_back(entry);
					  });

		std::sort(tmp.begin(), tmp.end(),
				  [](const ApacheAccessLogEntry& left, const ApacheAccessLogEntry& right )
				  {
				  	return left.ipInfo.country < right.ipInfo.country;
				  });
		auto last = std::unique(tmp.begin(), tmp.end(),
		                        [](const ApacheAccessLogEntry& left, const ApacheAccessLogEntry& right)
								{
									return left.ipInfo.country == right.ipInfo.country;
								});
		tmp.erase(last, tmp.end());

		_accessLogList = std::move(tmp);
	}

	if (!queryIspNames())
		return false;
	return true;
}

bool ApacheAccessLog::getItemsHtml(const std::string& country, std::string& result)
{
	result.clear();
	filterCountry(country);
	result = "<!DOCTYPE html>\n<html>\n" + getHtmlHead();
	result += 
"<body>\n"
"<table>\n"
"  <tr>\n"
"    <th>IP Address</th>\n"
"    <th>ISP Name</th>\n"
"    <th>Referer</th>\n"
"    <th>Datetime</th>\n"
"    <th>City</th>\n"
"    <th>Region Name</th>\n"
"    <th>User Agent</th>\n"
"  </tr>\n";

	std::for_each(_accessLogList.begin(), _accessLogList.end(),
	              [&result](const ApacheAccessLogEntry& entry)
				  {
				  	result += "  <tr>\n"
					          "    <td>" + entry.ipInfo.ipAddress + "</td>\n"
					          "    <td>" + entry.ipInfo.ispName + "</td>\n"
					          "    <td>" + entry.referer + "</td>\n"
					          "    <td>" + entry.datetimeStr + "</td>\n"
					          "    <td>" + entry.ipInfo.city + "</td>\n"
					          "    <td>" + entry.ipInfo.regionName + "</td>\n"
					          "    <td>" + entry.userAgent + "</td>\n";
				  });
	result += "</table>\n</body>\n</html>\n";
	return true;
}
