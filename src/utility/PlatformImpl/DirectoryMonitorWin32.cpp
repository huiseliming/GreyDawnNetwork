#include "utility/DirectoryMonitor.h"
#include <Windows.h>
#include "utility/SystemAbstraction.h"
#include "utility/Logger.h"

namespace GreyDawn
{

	DirectoryMonitor::DirectoryMonitor(DirectoryMonitor&& directory_monitor)
	{
		this->worker_ = std::move(directory_monitor.worker_);
		this->notify_function_ = std::move(directory_monitor.notify_function_);
		this->change_event_ = directory_monitor.change_event_;
		directory_monitor.change_event_ = INVALID_HANDLE_VALUE;
		this->stop_event_ = directory_monitor.stop_event_;
		directory_monitor.stop_event_ = NULL;
	}

	DirectoryMonitor& DirectoryMonitor::operator=(DirectoryMonitor&& directory_monitor)
	{
		if (this != std::addressof(directory_monitor))
		{
			this->worker_ = std::move(directory_monitor.worker_);
			this->notify_function_ = std::move(directory_monitor.notify_function_);
			this->change_event_ = directory_monitor.change_event_;
			directory_monitor.change_event_ = INVALID_HANDLE_VALUE;
			this->stop_event_ = directory_monitor.stop_event_;
			directory_monitor.stop_event_ = NULL;
		}
		return *this;
	}

	DirectoryMonitor::~DirectoryMonitor()
	{
		Stop();
		if (stop_event_ != NULL) 
			if(!CloseHandle(stop_event_))
				GD_LOG_OUTPUT_SYSTEM_ERROR();
	}

	bool DirectoryMonitor::Start(std::function<void()> notify_function, const std::string& path)
	{
		Stop();
		if (stop_event_ == NULL) {
			stop_event_ = CreateEvent(NULL, TRUE, FALSE, NULL);
			if (stop_event_ == NULL) {
				return false;
			}
		}
		else {
			if(!ResetEvent(stop_event_))
				GD_LOG_OUTPUT_SYSTEM_ERROR();
		}
		notify_function_ = std::move(notify_function);
		change_event_ = FindFirstChangeNotification(
			path.c_str(),
			FALSE,
			FILE_NOTIFY_CHANGE_FILE_NAME
			| FILE_NOTIFY_CHANGE_DIR_NAME
			| FILE_NOTIFY_CHANGE_LAST_WRITE
		);
		if (change_event_ == INVALID_HANDLE_VALUE) {
			return false;
		}
		worker_ = std::thread(&DirectoryMonitor::Run, this);
		return true;
	}

	void DirectoryMonitor::Stop()
	{
		if (!worker_.joinable()) {
			return;
		}
		if (!SetEvent(stop_event_))
			GD_LOG_OUTPUT_SYSTEM_ERROR();
		worker_.join();
		if(!FindCloseChangeNotification(change_event_))
			GD_LOG_OUTPUT_SYSTEM_ERROR();
	}

	void DirectoryMonitor::Run()
	{
		try{
			HANDLE event_handles[2] = { stop_event_, change_event_ };
			while (WaitForMultipleObjects(2, event_handles, FALSE, INFINITE) != 0) {
				if (FindNextChangeNotification(change_event_) == FALSE) {
					GD_LOG_OUTPUT_SYSTEM_ERROR();
					break;
				}
				notify_function_();
			}
		}catch(const std::exception& e){
            GD_LOG_ERROR("[std::exception>{}]", e.what());
        }catch(...){
            GD_LOG_ERROR("Unknow Exception");
        }
	}

}





