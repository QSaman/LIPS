#pragma once

#include <chrono>
#include <ratio>

using Hours = std::chrono::duration<long double, std::ratio<60 * 60>>;
using Minutes = std::chrono::duration<long double, std::ratio<60>>;
using Seconds = std::chrono::duration<long double>;

template<typename Duration, std::size_t MaxRequests>
class WebServiceTimer
{
public:
	WebServiceTimer() : _webServiceStatus(WebServiceStatus::Ready), _requestCounter(0) {}
	bool increaseRequestCounter();
	bool reachedRequestLimit();
	bool startTimer();
	void resetTimer();
	bool isTimerStarted() {return _webServiceStatus != WebServiceStatus::Ready;}
	std::size_t remainingRequests() {return MaxRequests >= _requestCounter ? MaxRequests - _requestCounter : 0;}
	std::size_t maximumRequests() {return MaxRequests;}
private:

	enum class WebServiceStatus
	{
		Ready,	//Web service is ready to send requests
		Counting,	//Web service has started sedning request but doesn't reach its limit
		Waiting	//Web service reached its limit and is not accept requests
	};
	using Clock = std::chrono::steady_clock;
	using TimePoint = typename Clock::time_point;

	WebServiceStatus _webServiceStatus;
	std::size_t _requestCounter;
	TimePoint _startPoint;
};

template<typename Duration, std::size_t MaxRequests>
bool WebServiceTimer<Duration, MaxRequests>::startTimer()
{
	if (_webServiceStatus != WebServiceStatus::Ready)
	{
		return false;
	}
	_webServiceStatus = WebServiceStatus::Counting;
	_startPoint = Clock::now();
	return true;
}

template<typename Duration, std::size_t MaxRequests>
bool WebServiceTimer<Duration, MaxRequests>::increaseRequestCounter()
{
	if (_webServiceStatus == WebServiceStatus::Ready)
	{
		return false;
	}
	else if (reachedRequestLimit())
	{
		if (_webServiceStatus == WebServiceStatus::Counting)
			_webServiceStatus = WebServiceStatus::Waiting;
		return false;
	}
	++_requestCounter;
	return true;
}

template<typename Duration, std::size_t MaxRequests>
void WebServiceTimer<Duration, MaxRequests>::resetTimer()
{
	_webServiceStatus = WebServiceStatus::Ready;
	_requestCounter = 0;
	startTimer();
}

template<typename Duration, std::size_t MaxRequests>
bool WebServiceTimer<Duration, MaxRequests>::reachedRequestLimit()
{
	auto now = Clock::now();

	static const Duration  unit(1);
	Duration duration = now - _startPoint;

	if (duration <= unit)
		return _requestCounter >= MaxRequests;
	else
		resetTimer();
	return false;
}
