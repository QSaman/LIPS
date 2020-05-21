#pragma once

#include <string>

struct IPAddressInfo
{
	IPAddressInfo(const std::string& ipAddress) : ipAddress {ipAddress} {}
	IPAddressInfo() = default;
	static bool isPublicIP(const std::string& ipAddress);

	std::string ipAddress;
	std::string country;
	std::string city;
	std::string regionName;
	std::string ispName;
	std::string organization;
	std::string as;
};
