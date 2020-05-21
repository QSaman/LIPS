#include "http_request_manager.hpp"
#include <algorithm>


HttpRequestManager::HttpRequestManager() :
	_countryWS([]()
	{
		//Since std::initializer_list do copies, it doesn't work for std::unique_ptr
		WebServiceList list;
		list.emplace_back(new FreeGeoIP());
		return list;
	}()),
	_ispNameWS([]()
	{
		WebServiceList list;
		list.emplace_back(new IpApi());
		return list;
	}())
{
}

bool HttpRequestManager::getCountry(IPAddressInfo& ipAddressInfo) const
{
	return getIpInfo(_countryWS, ipAddressInfo);
}

bool HttpRequestManager::getISP(IPAddressInfo& ipAddressInfo) const 
{
	return getIpInfo(_ispNameWS, ipAddressInfo);
}

bool HttpRequestManager::getIpInfo(const WebServiceList& ws, IPAddressInfo& ipAddressInfo) const
{
	bool res = false;
	for (const auto& webService : ws)
	{
		//Note that webService is a constant smart pointer, pointing to a non-constant object:
		if (webService->updateIpAddressInfo(ipAddressInfo))
		{
			res = true;
			break;
		}
	}
	return res;
}
