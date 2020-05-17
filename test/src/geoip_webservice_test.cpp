#define protected public
#include <geoip_webservice.hpp>
#undef protected

#include <gtest/gtest.h>

TEST(FreeGeoIP, TestProcessResponse)
{
	const std::string response = "{\"ip\":\"8.8.8.8\",\"country_code\":\"US\",\"country_name\":\"United States\",\"region_code\":\"\",\"region_name\":\"my_region_name\",\"city\":\"my_city\",\"zip_code\":\"\",\"time_zone\":\"America/Chicago\",\"latitude\":37.751,\"longitude\":-97.822,\"metro_code\":0}\n";
	IPAddressInfo ip;
	ip.ipAddress = "8.8.8.8";

	ASSERT_TRUE(ip.ipAddress == "8.8.8.8");

	FreeGeoIP ws;
	ws.processResponse(response, ip);
	ASSERT_TRUE(ip.country == "United States");
	ASSERT_TRUE(ip.regionName == "my_region_name");
	ASSERT_TRUE(ip.city == "my_city");
}
