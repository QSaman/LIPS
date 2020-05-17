#define private public
#include <geoip_webservice.hpp>
#undef private

#include <gtest/gtest.h>
#include <http_request_manager.hpp>


TEST(HttpRequestManager, CountryTest)
{
	HttpRequestManager manager;
	IPAddressInfo ip;
	ip.ipAddress = "8.8.8.8";
	ASSERT_TRUE(manager.getCountry(ip));
	ASSERT_TRUE(ip.country == "United States");
	ASSERT_TRUE(ip.ispName.empty());

	HttpSession::_cache->erase(ip.ipAddress);
}

TEST(HttpRequestManager, IspTest)
{
	HttpRequestManager manager;
	IPAddressInfo ip;
	ip.ipAddress = "8.8.8.8";

	ASSERT_TRUE(ip.ispName.empty());
	ASSERT_TRUE(manager.getISP(ip));
	ASSERT_TRUE(!ip.ispName.empty());

	HttpSession::_cache->erase(ip.ipAddress);
}
