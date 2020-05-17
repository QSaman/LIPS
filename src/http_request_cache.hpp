#pragma once

#include <string>
#include <unordered_map>

class HttpRequestCache
{
public:
	virtual bool findResponse(const std::string& ipAddress, std::string& response) = 0;
	virtual bool insert(const std::string& ipAddress, const std::string& response) = 0;
	virtual bool erase(const std::string& ipAddress) = 0;
	virtual ~HttpRequestCache() = default;
};


class HttpRequestMemoryCache : public HttpRequestCache
{
public:
	virtual bool findResponse(const std::string& ipAddress, std::string& response) override;
	virtual bool insert(const std::string& ipAddress, const std::string& response) override;
	virtual bool erase(const std::string& ipAddress) override;
private:
	using Cache = std::unordered_map<std::string, std::string>; 

	Cache _cache;
};
