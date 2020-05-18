#include "http_request_cache.hpp"

bool HttpRequestMemoryCache::findResponse(const std::string& url, std::string& response)
{
	auto iter = _cache.find(url);
	if (iter == _cache.end())
	{
		response.clear();
		return false;
	}
	response = iter->second;
	return true;
}

bool HttpRequestMemoryCache::insert(const std::string& url, const std::string& response)
{
	return _cache.insert(Cache::value_type(url, response)).second;
}

bool HttpRequestMemoryCache::erase(const std::string& url)
{
	return _cache.erase(url) > 0;
}
