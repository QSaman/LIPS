#include "apache_access_log.hpp"

#include <boost/date_time/gregorian/gregorian.hpp> 
#include <fstream>

void ApacheAccessLog::processFile(const std::string& fileName, const std::string& startDate, const std::string& endDate)
{
	using namespace boost::gregorian;
	std::ifstream fin;
	fin.exceptions(std::ifstream::badbit | std::ifstream::failbit);
	fin.open(fileName);

	std::string line;
	while (getline(fin, line))
	{
	}
}
