#include "http_request_cache.hpp"

bool HttpRequestMemoryCache::findResponse(const std::string& ipAddress, std::string& response)
{
	auto iter = _cache.find(ipAddress);
	if (iter == _cache.end())
	{
		response.clear();
		return false;
	}
	response = iter->second;
	return true;
}

bool HttpRequestMemoryCache::insert(const std::string& ipAddress, const std::string& response)
{
	return _cache.insert(Cache::value_type(ipAddress, response)).second;
}

bool HttpRequestMemoryCache::erase(const std::string& ipAddress)
{
	return _cache.erase(ipAddress) > 0;
}
