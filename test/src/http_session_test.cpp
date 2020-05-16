#include <gtest/gtest.h>
#include <geoip_webservice.hpp>
#include <string>
#include <list>

//For more information: https://freegeoip.app/
TEST(HttpSession, FreeGeoIP)
{
	const std::string expected = "{\"ip\":\"8.8.8.8\",\"country_code\":\"US\",\"country_name\":\"United States\",\"region_code\":\"\",\"region_name\":\"\",\"city\":\"\",\"zip_code\":\"\",\"time_zone\":\"America/Chicago\",\"latitude\":37.751,\"longitude\":-97.822,\"metro_code\":0}\n";
	HttpSession session;
	//https://freegeoip.app/json/8.8.8.8
	session.setUrl("https://freegeoip.app/json");
	std::list<std::string> list = {"accept: application/json", "content-type: application/json"};
	session.setHttpHeader(list);
	auto res = session.sendRequest("8.8.8.8");
	ASSERT_TRUE(res == expected);
}
 
//For more information: https://ip-api.com/docs/api:json
TEST(HttpSession, IpApi)
{
	const std::string expected = "{\"status\":\"success\",\"country\":\"United States\",\"regionName\":\"Virginia\",\"city\":\"Ashburn\",\"isp\":\"Google LLC\",\"as\":\"AS15169 Google LLC\",\"query\":\"8.8.8.8\"}";
	HttpSession session;
	//http://ip-api.com/json/8.8.8.8?fields=query,status,country,regionName,city,isp,as
	session.setUrl("http://ip-api.com/json/");
	session.setGetParameters("fields=query,status,country,regionName,city,isp,as");
	auto res = session.sendRequest("8.8.8.8");
	std::cout << res;
	ASSERT_TRUE(res == expected);
}
