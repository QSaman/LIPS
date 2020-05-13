#include "apache_access_log.hpp"

#include <boost/date_time/gregorian/gregorian.hpp> 
#include <boost/exception/exception.hpp>
#include <fstream>
#include <iostream>
#include <cctype>

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

void ApacheAccessLog::processFile(const std::string& fileName, const std::string& startDate, const std::string& endDate)
{
	using namespace boost::gregorian;
	date start(from_simple_string(startDate));
	date end(from_simple_string(endDate));

	std::ifstream fin;
	fin.exceptions(std::ifstream::badbit | std::ifstream::failbit);
	fin.open(fileName);

	std::string line;
	while (getline(fin, line))
	{
		auto tokens = tokenizeLogEntry(line);
		if (tokens.size() <= 7 || tokens[IpIndex] == "::1")
			continue;
		date current(from_uk_string(extractDateFromDateTime(tokens[DateTimeIndex])));
		if (current < start)
			continue;
		if (current > end)
			return;
		ApacheAccessLogEntry logEntry;
		logEntry.ipQuery.setIpAddress(tokens[IpIndex]);
		logEntry.datetimeStr = tokens[DateTimeIndex];
		logEntry.referer = tokens[RefererIndex];
		logEntry.userAgent = tokens[UserAgentIndex];
		_accessLogList.push_back(logEntry);
	}
}
