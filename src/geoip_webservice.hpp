#pragma once

#include <curlpp/Easy.hpp>
#include <list>
#include <memory>

#include "ip_address.hpp"
#include "web_service_timer.hpp"
#include "http_request_cache.hpp"

class HttpSession
{
public:
	void setUrl(const std::string& url);
	void setHttpHeader(const std::list<std::string>& httpHeader); 
	void setGetParameters(const std::string& parameters) {_getParas = parameters;}
	std::string sendRequest(const std::string& ipAddress);
private:
	friend class GeoIPWebService;
    curlpp::Easy _request;
	std::string _url;
	std::string _getParas;
	static std::unique_ptr<HttpRequestCache> _cache;
};

class GeoIPWebService
{
public:
	~GeoIPWebService() = default;
	bool updateIpAddressInfo(IPAddressInfo& ipAddressInfo);
protected:
	virtual bool increaseTimerCounter() = 0;
	virtual bool processResponse(const std::string& response, IPAddressInfo& ipAddress) = 0;
	HttpSession _httpSession;
};

//https://freegeoip.app/
class FreeGeoIP : public GeoIPWebService
{
public:
	FreeGeoIP();
protected:
	virtual bool increaseTimerCounter() override;
	virtual bool processResponse(const std::string& response, IPAddressInfo& ipAddress) override;
private:
	static WebServiceTimer<Hours, 15000> _timer;
};

//https://ip-api.com/docs/api:json
class IpApi : public GeoIPWebService
{
public:
	IpApi();
protected:
	virtual bool increaseTimerCounter() override;
	virtual bool processResponse(const std::string& response, IPAddressInfo& ipAddress) override;
private:
	static WebServiceTimer<Minutes, 45> _timer;
};
