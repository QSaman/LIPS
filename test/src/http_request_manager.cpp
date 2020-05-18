#define private public
#define protected public
#include <http_request_manager.hpp>
#include <geoip_webservice.hpp>
#undef protected
#undef private

#include <gtest/gtest.h>


TEST(HttpRequestManager, CountryTest)
{
	HttpRequestManager manager;
	IPAddressInfo ip;
	ip.ipAddress = "8.8.8.8";
	ASSERT_TRUE(manager.getCountry(ip));
	ASSERT_TRUE(ip.country == "United States");
	ASSERT_TRUE(ip.ispName.empty());

	for (auto& ws : manager._countryWS)
	HttpSession::_cache->erase(ws->_httpSession.generateUrl(ip.ipAddress));
}

TEST(HttpRequestManager, IspTest)
{
	HttpRequestManager manager;
	IPAddressInfo ip;
	ip.ipAddress = "8.8.8.8";

	ASSERT_TRUE(ip.ispName.empty());
	ASSERT_TRUE(manager.getISP(ip));
	ASSERT_TRUE(!ip.ispName.empty());

	for (auto& ws : manager._ispNameWS)
	HttpSession::_cache->erase(ws->_httpSession.generateUrl(ip.ipAddress));
}
