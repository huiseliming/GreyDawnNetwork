#include <utility/Utility.h>
#include <filesystem>
using namespace GreyDawn;


class TestService : public Service
{
public:
	// This case the log output to C:\Windows\System32\logs
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

	std::string GetServiceName() override
	{
		return "GreyDawnServiceTest";
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

int main(int argc, char* argv[])
{
	TestService ts;
	if (argc > 1 && !strcmp(argv[1],"install"))
	{
		ts.Install();
	}
	else if (argc > 1 && !strcmp(argv[1], "uninstall"))
	{
		ts.Uninstall();
	}
	else 
	{
		int exit_code = ts.Start();
		return exit_code;
	}
}







