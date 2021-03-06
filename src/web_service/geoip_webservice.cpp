#include "geoip_webservice.hpp"

#include <boost/property_tree/ptree_fwd.hpp>
#include <chrono>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <sstream>
#include <cmath>

#include <boost/property_tree/json_parser.hpp>

std::unique_ptr<HttpRequestCache> HttpSession::_cache = std::unique_ptr<HttpRequestCache>(new HttpRequestMemoryCache());

WebServiceTimer<Hours, 15000>  FreeGeoIP::_timer;
WebServiceTimer<Minutes, 45>   IpApi::_timer;

namespace
{
	template<typename BinaryFunction>
	void parseJson(const std::string& jsonString, BinaryFunction func)
	{
		std::istringstream iss(jsonString);
		boost::property_tree::ptree json;
		boost::property_tree::read_json(iss, json);

		for (auto item : json)
		{
			auto value = item.second.get_value<std::string>();
			if (value.empty())
				continue;
			if (!func(item.first, item.second.get_value<std::string>()))
				return;
		}
	}
}

void HttpSession::setUrl(const std::string& url)
{
	_url = url;
	if (_url.back() != '/')
		_url += "/";
}

void HttpSession::setHttpHeader(const std::list<std::string>& httpHeader)
{
    _request.setOpt(new curlpp::options::HttpHeader(httpHeader));
}

std::string HttpSession::generateUrl(const std::string& ipAddress)
{
	const std::string url = [this, ipAddress]()
	{
		std::string url = _url + ipAddress;
		if (_getParas.empty())
			return url;
		return url + "?" + _getParas;
	}();
	return url;
}

std::string HttpSession::sendRequest(const std::string& ipAddress)
{
	std::string response;
	const std::string url = generateUrl(ipAddress);
	if (_cache->findResponse(url, response))
		return response;

	std::ostringstream oss;
    _request.setOpt(new curlpp::options::Url(url));
	oss << _request << std::flush;
	response = oss.str();
	_cache->insert(url, response);
	return response;
}

bool GeoIPWebService::updateIpAddressInfo(IPAddressInfo& ipAddressInfo)
{
	std::string response;
	const auto url = _httpSession.generateUrl(ipAddressInfo.ipAddress);
	if (!HttpSession::_cache->findResponse(url, response))
	{
		if (increaseTimerCounter())
			response = _httpSession.sendRequest(ipAddressInfo.ipAddress);
		else
			return false;
	}
	bool ret = processResponse(response, ipAddressInfo);
	if (!ret)
		HttpSession::_cache->erase(url);
	return ret;
}

FreeGeoIP::FreeGeoIP()
{
	/*
	 * curl --request GET \
            --url https://freegeoip.app/json/ \
            --header 'accept: application/json' \
            --header 'content-type: application/json'  
	 * */
	_httpSession.setUrl("https://freegeoip.app/json");
	std::list<std::string> list = {"accept: application/json", "content-type: application/json"};
	_httpSession.setHttpHeader(list);
}

bool FreeGeoIP::increaseTimerCounter()
{
	_timer.startTimer();
	return _timer.increaseRequestCounter();
}

std::size_t FreeGeoIP::remainingRequests()
{
	return _timer.remainingRequests();
}

std::size_t FreeGeoIP::maximumRequests()
{
	return _timer.maximumRequests();
}

void FreeGeoIP::resetTimer()
{
	_timer.resetTimer();
}

bool FreeGeoIP::processResponse(const std::string& response, IPAddressInfo& ipAddress)
{
	bool updated = false;

	parseJson(response, [&updated, &ipAddress, this](const std::string& key, const std::string& value)
	{
		if (key == "country_name")
		{
			ipAddress.country = value;
		}
		else if (key == "region_name")
		{
			ipAddress.regionName = value;
		}
		else if (key == "city")
		{
			ipAddress.city = value;
		}
		else if (key == "time_zone")
		{
			updated = !value.empty();	//For example 66.249.82.142 doesn't have country but only continent
		}
		return true;
	});
	return updated;
}

IpApi::IpApi()
{
	_httpSession.setUrl("http://ip-api.com/json/");
	_httpSession.setGetParameters("fields=status,country,regionName,city,isp,org,as,asname,query");
}

bool IpApi::increaseTimerCounter()
{
	_timer.startTimer();
	return _timer.increaseRequestCounter();
}

std::size_t IpApi::remainingRequests()
{
	return _timer.remainingRequests();
}

std::size_t IpApi::maximumRequests()
{
	return _timer.maximumRequests();
}

void IpApi::resetTimer()
{
	_timer.resetTimer();
}

bool IpApi::processResponse(const std::string& response, IPAddressInfo& ipAddress)
{
	bool updated = false;

	parseJson(response, [&updated, &ipAddress, this](const std::string& key, const std::string& value)
	{
		if (key == "status" && value != "success")
		{
			updated = false;
			return false;
		}

		if (key == "country")
		{
			ipAddress.country = value;
			updated = true;
		}
		else if (key == "regionName")
		{
			ipAddress.regionName = value;
			updated = true;
		}
		else if (key == "city")
		{
			ipAddress.city = value;
			updated = true;
		}
		else if (key == "isp")
		{
			ipAddress.ispName = value;
			updated = true;
		}
		else if (key == "org")
		{
			ipAddress.organization = value;
			updated = true;
		}
		else if (key == "as")
		{
			ipAddress.as = value;
			updated = true;
		}
		else if (key == "asname")
		{
			ipAddress.asName = value;
			updated = true;
		}
		return true;
	});

	return updated;
}
