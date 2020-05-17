#include "geoip_webservice.hpp"

#include <boost/property_tree/ptree_fwd.hpp>
#include <chrono>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <sstream>
#include <cmath>

#include <boost/property_tree/json_parser.hpp>

WebServiceTimer<Hours, 15000> FreeGeoIP::_timer;

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

std::string HttpSession::sendRequest(const std::string& ipAddress)
{
	std::ostringstream oss;
	const std::string url = [this, ipAddress]()
	{
		std::string url = _url + ipAddress;
		if (_getParas.empty())
			return url;
		return url + "?" + _getParas;
	}();
    _request.setOpt(new curlpp::options::Url(url));
	oss << _request << std::flush;
	return oss.str();
}

bool GeoIPWebService::updateIpAddressInfo(IPAddressInfo& ipAddressInfo)
{
	if (increaseTimerCounter())
		return false;
	auto response = _httpSession.sendRequest(ipAddressInfo.ipAddress);
	return processResponse(response, ipAddressInfo);
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

bool FreeGeoIP::processResponse(const std::string& response, IPAddressInfo& ipAddress)
{
	std::istringstream iss(response);
	boost::property_tree::ptree json;
	boost::property_tree::read_json(iss, json);
	for (auto item : json)
	{
		auto value = item.second.get_value<std::string>();
		if (value.empty())
			continue;
		if (item.first == "country_name")
			ipAddress.country = value;
		else if (item.first == "region_name")
			ipAddress.regionName = value;
		else if (item.first == "city")
			ipAddress.city = value;
	}
	return true;
}
