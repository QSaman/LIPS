#define protected public
#define private public
#include <geoip_webservice.hpp>
#undef private
#undef protected

#include <gtest/gtest.h>

#include <memory>
#include <vector>
#include <algorithm>

TEST(FreeGeoIP, TestProcessResponse)
{
	const std::string response = "{\"ip\":\"8.8.8.8\",\"country_code\":\"US\",\"country_name\":\"United States\",\"region_code\":\"\",\"region_name\":\"my_region_name\",\"city\":\"my_city\",\"zip_code\":\"\",\"time_zone\":\"America/Chicago\",\"latitude\":37.751,\"longitude\":-97.822,\"metro_code\":0}\n";
	IPAddressInfo ip;
	ip.ipAddress = "8.8.8.8";

	ASSERT_TRUE(ip.ipAddress == "8.8.8.8");

	FreeGeoIP ws;
	ASSERT_TRUE(ws.processResponse(response, ip));
	ASSERT_TRUE(ip.country == "United States");
	ASSERT_TRUE(ip.regionName == "my_region_name");
	ASSERT_TRUE(ip.city == "my_city");
}

TEST(IpApi, TestProcessResponse)
{
	const std::string response = "{\"status\":\"success\",\"country\":\"United States\",\"regionName\":\"Virginia\",\"city\":\"Ashburn\",\"isp\":\"Google LLC\",\"as\":\"AS15169 Google LLC\",\"query\":\"8.8.8.8\"}";

	IPAddressInfo ip;
	ip.ipAddress = "8.8.8.8";
	ASSERT_TRUE(ip.ipAddress == "8.8.8.8");

	IpApi ws;
	ASSERT_TRUE(ws.processResponse(response, ip));
	ASSERT_TRUE(ip.country == "United States");
	ASSERT_TRUE(ip.regionName == "Virginia");
	ASSERT_TRUE(ip.city == "Ashburn");
	ASSERT_TRUE(ip.ispName == "Google LLC");
}

TEST(FreeGeoIP, TestProcessResponseFailed)
{
	const std::string response = "{\"ip\":\"8.8.8.8\",\"country_code\":\"US\",\"country_name\":\"\",\"region_code\":\"\",\"region_name\":\"\",\"city\":\"\",\"zip_code\":\"\",\"time_zone\":\"America/Chicago\",\"latitude\":37.751,\"longitude\":-97.822,\"metro_code\":0}\n";
	IPAddressInfo ip;
	ip.ipAddress = "8.8.8.8";

	ASSERT_TRUE(ip.ipAddress == "8.8.8.8");

	FreeGeoIP ws;
	ASSERT_FALSE(ws.processResponse(response, ip));
}

TEST(IpApi, TestProcessResponseFailed)
{
	const std::string response = "{\"status\":\"failed\",\"country\":\"United States\",\"regionName\":\"Virginia\",\"city\":\"Ashburn\",\"isp\":\"Google LLC\",\"as\":\"AS15169 Google LLC\",\"query\":\"8.8.8.8\"}";

	IPAddressInfo ip;
	ip.ipAddress = "8.8.8.8";
	ASSERT_TRUE(ip.ipAddress == "8.8.8.8");

	IpApi ws;
	ASSERT_FALSE(ws.processResponse(response, ip));
}

TEST(GeoIPWebService, TestCacheManagement)
{
	//Note that isp field is intentionally empty
	const std::string invalidResponse = "{\"status\":\"success\",\"country\":\"United States\",\"regionName\":\"Virginia\",\"city\":\"Ashburn\",\"isp\":\"\",\"as\":\"AS15169 Google LLC\",\"query\":\"8.8.8.8\"}";

	using WebServicePtr = std::unique_ptr<GeoIPWebService>;
	using WebServiceList = std::vector<WebServicePtr>;

	WebServiceList wsList;
	wsList.push_back(WebServicePtr(new FreeGeoIP()));
	wsList.push_back(WebServicePtr(new IpApi()));

	IPAddressInfo ip;
	ip.ipAddress = "8.8.8.8";

	ASSERT_TRUE(ip.ipAddress == "8.8.8.8");

	ASSERT_TRUE(wsList[0]->remainingRequests() == wsList[0]->maximumRequests());
	ASSERT_TRUE(wsList[0]->updateIpAddressInfo(ip));
	ASSERT_TRUE(wsList[0]->remainingRequests() == (wsList[0]->maximumRequests() - 1));
	ASSERT_TRUE(ip.ispName.empty());

	ASSERT_TRUE(wsList[0]->updateIpAddressInfo(ip));
	ASSERT_TRUE(ip.ispName.empty());
	//The previous request is cached, no need to increase the counter:
	ASSERT_TRUE(wsList[0]->remainingRequests() == (wsList[0]->maximumRequests() - 1));

	ASSERT_TRUE(wsList[1]->remainingRequests() == wsList[1]->maximumRequests());
	ASSERT_TRUE(wsList[1]->updateIpAddressInfo(ip));
	ASSERT_FALSE(ip.ispName.empty());

	std::for_each(wsList.begin(), wsList.end(), [](const WebServicePtr& ptr)
				  {
					HttpSession::_cache->erase(ptr->_httpSession.generateUrl("8.8.8.8"));
				  });
}
