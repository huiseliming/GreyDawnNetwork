#include <gtest/gtest.h>
#include <utility/Utility.h>

using namespace GreyDawn;


class TestService : public Service
{
public:
	int Run() override
	{
		{
			std::lock_guard<std::mutex> lock(wait_mutex_);
			change_target_ = false;
		}
		GD_LOG_INFO("{} Running", GetServiceName());
		WaitForSingleObject(stop_event_, INFINITE);
		GD_LOG_INFO("{} Stoped", GetServiceName());
		{
			std::lock_guard<std::mutex> lock(wait_mutex_);
			change_target_ = true;
		}
		return EXIT_SUCCESS;
	}

	bool AwaitChange() 
	{
		std::unique_lock<std::mutex> lock(wait_mutex_);
		check_condition_variable_.wait_for(lock, std::chrono::seconds(), [this] { return change_target_; });
		return change_target_;
	}
	
	bool change_target_ =false;
	std::mutex wait_mutex_;
	std::condition_variable check_condition_variable_;
	
};



TEST(tests, ServiceTests) 
{
	{
		TestService ts;
		ASSERT_TRUE(ts.Start() == EXIT_SUCCESS);
		std::this_thread::sleep_for(std::chrono::seconds(1));
		ts.Stop();
		ASSERT_TRUE(ts.AwaitChange());
	}
}







