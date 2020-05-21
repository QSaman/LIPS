#include "apache_access_log.hpp"
#include "cli_options.hpp"

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

	CountryFrequencyList constructCountryFreq(const ApacheAccessLog::AccessLogList& accessLogList)
	{
		CountryFrequencyList ret;
		std::size_t i, j;
		//Assuming the list is sorted by country names:
		for (i = 0; i < accessLogList.size(); i = j)
		{
			for (j = i + 1; j < accessLogList.size() && accessLogList[i].ipInfo.country == accessLogList[j].ipInfo.country; ++j);
			CountryFrequency cf;
			cf.first = accessLogList[i].ipInfo.country;
			cf.second = j - i;
			ret.push_back(cf);
		}
		std::sort(ret.begin(), ret.end(), [](const CountryFrequency& left, const CountryFrequency& right)
				  {
				  	return left.second > right.second;
				  });
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

	std::string::size_type j = 1;
	for (std::string::size_type i = 0; i < line.size(); i = j + 1, j += 2)
	{
		if (std::isspace(line[i]))
		{
			continue;
		}

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
		if (line[i] == '"' || line[i] == '[')
			tokens.push_back(line.substr(i + 1, j - i - 2));
		else
			tokens.push_back(line.substr(i, j - i));
	}
	return tokens;
}

std::string ApacheAccessLog::extractDateFromDateTime(const std::string& dateTime)
{
	auto colonIndex = dateTime.find(':');
	return dateTime.substr(0, colonIndex);
}

std::string ApacheAccessLog::extractHourMinuteFromDateTime(const std::string& dateTime)
{
	auto colonIndex = dateTime.find(':');
	return dateTime.substr(colonIndex + 1, 5);
}

bool ApacheAccessLog::processFile(const std::string& fileName, const boost::gregorian::date& startDate, const boost::gregorian::date& endDate)
{
	std::ifstream fin;
	//fin.exceptions(std::ifstream::badbit | std::ifstream::failbit);
	fin.exceptions(std::ifstream::badbit);
	fin.open(fileName);
	return processStream(fin, startDate, endDate);
}

bool ApacheAccessLog::populateIPFields()
{
	if (verbose)
		std::cout << "Fetching countries for IP list..." << std::endl;
	if (!queryCountries())
		return false;
	return queryIspNames();
}

bool ApacheAccessLog::processStream(std::istream& in, const boost::gregorian::date& startDate, const boost::gregorian::date& endDate)
{
	using namespace boost::gregorian;

	std::unordered_set<std::string> mark;

	std::string line;
	while (getline(in, line))
	{
		if (line.empty())
			continue;
		auto tokens = tokenizeLogEntry(line);
		if (tokens.size() <= 7 || !IPAddressInfo::isPublicIP(tokens[IpIndex]))
			continue;
		date current(from_uk_string(extractDateFromDateTime(tokens[DateTimeIndex])));
		if (!startDate.is_not_a_date() && current < startDate)
			continue;
		if (!endDate.is_not_a_date() && current > endDate)
			return populateIPFields();
		if (std::find(_excludedUsers.begin(), _excludedUsers.end(), tokens[UserIndex]) != _excludedUsers.end())
			continue;
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
	return populateIPFields();
}

bool ApacheAccessLog::queryCountries()
{
	for (auto& val : _accessLogList)
		if (!_httpRequestManager.getCountry(val.ipInfo))
			return false;
	std::sort(_accessLogList.begin(), _accessLogList.end(),
	          [](const ApacheAccessLogEntry& left, const ApacheAccessLogEntry& right)
			  {
			  	return left.ipInfo.country < right.ipInfo.country;
			  });
	return true;
}

bool ApacheAccessLog::queryIspNames()
{
	if (_country.empty())
		return true;

	for (auto& val : _accessLogList)
	{
		if (val.ipInfo.country != _country)
			continue;
		if (!_httpRequestManager.getISP(val.ipInfo))
			return false;
	}
	return true;
}

std::string ApacheAccessLog::getSummaryByCountry() const
{
	std::string result;

	if (_accessLogList.size() == 0)
		return result;

	auto list = constructCountryFreq(_accessLogList);
	std::for_each(list.begin(), list.end(),
	              [&result](const CountryFrequency& cf)
				  {
				  	result += cf.first + ": " + std::to_string(cf.second) + "\n";
				  });
	return result;
}

std::string ApacheAccessLog::getSummaryByCountryHtml() const
{
	std::string result;

	if (_accessLogList.size() == 0)
		return result;

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
	return result;
}

std::string ApacheAccessLog::getItemsHtml() const
{
	std::string result;
	const AccessLogList list = [this]()
	{
		AccessLogList list;
		for (const auto& entry : _accessLogList)
			if (entry.ipInfo.country == _country)
				list.push_back(entry);
		return list;
	}();

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
"    <th>As</th>\n"
"    <th>Organization</th>\n"
"    <th>User Agent</th>\n"
"  </tr>\n";

	std::for_each(list.begin(), list.end(),
	              [&result](const ApacheAccessLogEntry& entry)
				  {
				  	result += "  <tr>\n"
					          "    <td>" + entry.ipInfo.ipAddress + "</td>\n"
					          "    <td>" + entry.ipInfo.ispName + "</td>\n"
					          "    <td>" + entry.referer + "</td>\n"
					          "    <td>" + entry.datetimeStr + "</td>\n"
					          "    <td>" + entry.ipInfo.city + "</td>\n"
					          "    <td>" + entry.ipInfo.regionName + "</td>\n"
					          "    <td>" + entry.ipInfo.as + "</td>\n"
					          "    <td>" + entry.ipInfo.organization + "</td>\n"
					          "    <td>" + entry.userAgent + "</td>\n";
				  });
	result += "</table>\n</body>\n</html>\n";
	return result;
}
