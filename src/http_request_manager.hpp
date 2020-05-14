#pragma once

#include <string>
#include <curlpp/Easy.hpp>
#include <list>

class HttpSession
{
public:
	void setUrl(const std::string& url);
	void setHttpHeader(const std::list<std::string>& httpHeader); 
	std::string sendRequest();
private:
    curlpp::Easy _request;
};

class HttpRequestManager
{
public:
private:
};
