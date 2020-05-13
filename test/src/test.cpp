#include <gtest/gtest.h>
#include <fstream>

#include <apache_access_log.hpp>

TEST(ApacheAccessLog, Exceptions)
{
	ApacheAccessLog accesLog;
	ASSERT_THROW(accesLog.processFile("invalid_file_name", "2020-12-01", "2020-12-02"), std::ios_base::failure);
}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
