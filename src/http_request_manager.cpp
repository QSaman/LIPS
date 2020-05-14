#include "http_request_manager.hpp" 

#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <sstream>

void HttpSession::setUrl(const std::string& url)
{
    _request.setOpt(new curlpp::options::Url(url));
}

void HttpSession::setHttpHeader(const std::list<std::string>& httpHeader)
{
    _request.setOpt(new curlpp::options::HttpHeader(httpHeader));
}

std::string HttpSession::sendRequest()
{
	std::ostringstream oss;
	oss << _request << std::flush;
	return oss.str();
}
