#define private public
#include <web_service_timer.hpp>
#undef private

#include <gtest/gtest.h>
#include <chrono>
#include <thread>

TEST(WebServiceTimer, Test20RequestsPerSecond)
{
	constexpr std::size_t MaxRequests = 20;
	using Timer = WebServiceTimer<Seconds, MaxRequests>;
	Timer timer;
	auto start = Timer::Clock::now();
	timer.startTimer();
	for (std::size_t i = 0; i < MaxRequests; ++i)
		ASSERT_TRUE(timer.increaseRequestCounter());
	ASSERT_FALSE(timer.increaseRequestCounter());

	auto now = Timer::Clock::now();
	std::this_thread::sleep_for(Seconds(1) - (now - start)); 

	for (std::size_t i = 0; i < MaxRequests; ++i)
		ASSERT_TRUE(timer.increaseRequestCounter());
	ASSERT_FALSE(timer.increaseRequestCounter());
}

TEST(WebServiceTimer, Test20RequestsPer2Seconds)
{
	constexpr std::size_t MaxRequests = 20;
	using Timer = WebServiceTimer<Seconds, MaxRequests>;
	Timer timer;
	auto start = Timer::Clock::now();
	timer.startTimer();

	constexpr std::size_t halfRequests = MaxRequests / 2;
	constexpr std::size_t remainedRequests = MaxRequests - halfRequests;

	ASSERT_TRUE(timer.remainingRequests() == timer.maximumRequests());
	for (std::size_t i = 0; i < halfRequests; ++i)
		ASSERT_TRUE(timer.increaseRequestCounter());
	ASSERT_TRUE(timer.remainingRequests() == remainedRequests);

	auto now = Timer::Clock::now();
	std::this_thread::sleep_for(Seconds(1) - (now - start)); 
	ASSERT_TRUE(timer.increaseRequestCounter());
	//After 1 seconds, the timer should reset it's counter:
	ASSERT_TRUE(timer.remainingRequests() == timer.maximumRequests() - 1);
}

