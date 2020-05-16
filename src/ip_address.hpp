#pragma once

#include <string>

class IPAddressQuery
{
public:
	IPAddressQuery(const std::string& ipAddress) : _ipAddress {ipAddress} {}
	IPAddressQuery() = default;
	static bool isPublicIP(const std::string& ipAddress);
	void setIpAddress(const std::string& ipAddress) { _ipAddress = ipAddress; }
	const std::string& ipAddress() { return _ipAddress; }
	const std::string& country() { return _country; }
	const std::string& ispName() { return _ispName; }
private:
	std::string _ipAddress;
	std::string _country;
	std::string _ispName;
};
