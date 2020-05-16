#include "ip_address.hpp" 

bool IPAddressQuery::isPublicIP(const std::string& ipAddress)
{
	if (ipAddress == "::1")
		return false;
	if (ipAddress[0] == '1' && ipAddress[1] == '2' && ipAddress[2] == '7')	//127.0.0.0/8
		return false;
	if (ipAddress[0] == '1' && ipAddress[1] == '0')	//10.0.0.0/8
		return false;
	if (ipAddress.substr(0, 7) == "192.168")	//192.168.0.0/16
		return false;
	auto isTwentyBitBlock = [](const std::string& ipAddress)	//172.16.0.0/12
	{
		if (ipAddress[0] != 1 || ipAddress[1] != 7 || ipAddress[2] != 2)
			return false;
		int num = std::stoi(ipAddress.substr(4, 2));
		return num >= 16 && num <= 31;
	};
	return !isTwentyBitBlock(ipAddress);
}
