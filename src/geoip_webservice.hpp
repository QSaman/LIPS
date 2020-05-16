#pragma once

#include <curlpp/Easy.hpp>
#include <list>

#include "ip_address.hpp"
#include "web_service_timer.hpp"

class HttpSession
{
public:
	void setUrl(const std::string& url);
	void setHttpHeader(const std::list<std::string>& httpHeader); 
	void setGetParameters(const std::string& parameters) {_getParas = parameters;}
	std::string sendRequest(const std::string& ipAddress);
private:
    curlpp::Easy _request;
	std::string _url;
	std::string _getParas;
};

class GeoIPWebService
{
public:
	~GeoIPWebService() = default;
	bool updateIpAddressQuery(IPAddressQuery& ipAddressQuery);
protected:
	virtual bool reachedRequestLimit() = 0;
	virtual bool processResponse(const std::string& response, IPAddressQuery& ipAddress) = 0;
	HttpSession _httpSession;
};

class FreeGeoIP : public GeoIPWebService
{
public:
	FreeGeoIP();
protected:
	virtual bool reachedRequestLimit() override;
	virtual bool processResponse(const std::string& response, IPAddressQuery& ipAddress) override;
private:
	static WebServiceTimer<Hours, 15000> _timer;
};
