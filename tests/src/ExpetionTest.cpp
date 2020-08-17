#include <gtest/gtest.h>
#include <utility/Utility.h>
#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#endif


TEST(tests,ExceptionTest)
{
	ASSERT_THROW(THROW_EXCEPT("ExceptionTest"), GreyDawn::Exception);
}

TEST(tests, SystemErrorExceptionTest)
{
	ASSERT_THROW(THROW_SYSTEM_ERROR_EXCEPT("SystemErrorExceptionTest"), GreyDawn::SystemErrorExpetion);
#ifdef _WIN32
	ASSERT_THROW(THROW_SYSTEM_ERROR_IF_FAILED(!CloseHandle(NULL)), GreyDawn::SystemErrorExpetion);
#elif __linux__
#endif
}










