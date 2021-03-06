#define private public
#include <web_service/geoip_webservice.hpp>
#undef private

#include <gtest/gtest.h>
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

	HttpSession::_cache->erase(session.generateUrl("8.8.8.8"));
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
	ASSERT_TRUE(res == expected);

	HttpSession::_cache->erase(session.generateUrl("8.8.8.8"));
}

TEST(HttpSession, HttpRequestCache)
{
	const std::string expected = "{\"ip\":\"8.8.8.8\",\"country_code\":\"US\",\"country_name\":\"United States\",\"region_code\":\"\",\"region_name\":\"\",\"city\":\"\",\"zip_code\":\"\",\"time_zone\":\"America/Chicago\",\"latitude\":37.751,\"longitude\":-97.822,\"metro_code\":0}\n";

	HttpSession session;
	//https://freegeoip.app/json/8.8.8.8
	session.setUrl("https://freegeoip.app/json");
	std::list<std::string> list = {"accept: application/json", "content-type: application/json"};
	session.setHttpHeader(list);
	auto res = session.sendRequest("8.8.8.8");
	ASSERT_TRUE(res == expected);

	const std::string expected2 = "{\"status\":\"success\",\"country\":\"United States\",\"regionName\":\"Virginia\",\"city\":\"Ashburn\",\"isp\":\"Google LLC\",\"as\":\"AS15169 Google LLC\",\"query\":\"8.8.8.8\"}";
	HttpSession session2;
	session2.setUrl("http://ip-api.com/json/");
	session2.setGetParameters("fields=query,status,country,regionName,city,isp,as");
	res = session2.sendRequest("8.8.8.8");
	ASSERT_TRUE(res == expected2);

	//Note that country_name is modified
	const std::string expectedModified = "{\"ip\":\"8.8.8.8\",\"country_code\":\"US\",\"country_name\":\"United States**\",\"region_code\":\"\",\"region_name\":\"\",\"city\":\"\",\"zip_code\":\"\",\"time_zone\":\"America/Chicago\",\"latitude\":37.751,\"longitude\":-97.822,\"metro_code\":0}\n";


	HttpSession::_cache->erase(session.generateUrl("8.8.8.8"));
	HttpSession::_cache->insert(session.generateUrl("8.8.8.8"), expectedModified);
	res = session.sendRequest("8.8.8.8");
	ASSERT_TRUE(res == expectedModified);

	HttpSession::_cache->erase(session.generateUrl("8.8.8.8"));
	HttpSession::_cache->erase(session2.generateUrl("8.8.8.8"));
}
