#pragma once

#include "ip_address.hpp"
#include "geoip_webservice.hpp"

#include <memory>
#include <vector>

class HttpRequestManager
{
public:
	HttpRequestManager();
	bool getCountry(IPAddressInfo& ipAddressInfo) const;
	bool getISP(IPAddressInfo& ipAddressInfo) const;
private:
	using WebServicePtr = std::unique_ptr<GeoIPWebService>;
	using WebServiceList = std::vector<WebServicePtr>;

	bool getIpInfo(const WebServiceList& ws, IPAddressInfo& ipAddressInfo) const;

	const WebServiceList _countryWS;
	const WebServiceList _ispNameWS;
};
