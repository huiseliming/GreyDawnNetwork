#pragma once
#include <string>
#include <array>
#include <memory>
#include <functional>
#include <thread>
#ifdef _WIN32
#include <Windows.h>
#endif


namespace GreyDawn 
{

	class DirectoryMonitor
	{
    public:
        DirectoryMonitor() = default;
        DirectoryMonitor(const DirectoryMonitor&) = delete;
        DirectoryMonitor(DirectoryMonitor&& directory_monitor);
        DirectoryMonitor& operator=(const DirectoryMonitor&) = delete;
        DirectoryMonitor& operator=(DirectoryMonitor&& directory_monitor);
        ~DirectoryMonitor();
        bool Start(std::function<void()> notify_function, const std::string& path);
        void Stop();
    private:
        void Run();
    private:
        std::thread worker_;
        std::function<void()> notify_function_;
#ifdef _WIN32
        HANDLE change_event_ = INVALID_HANDLE_VALUE;
        HANDLE stop_event_ = NULL;
#endif

	};

}

