#include "utility/DirectoryMonitor.h"
#include "utility/SystemAbstraction.h"
#include "utility/Logger.h"
#include <sys/inotify.h>
#include <sys/types.h>
#include <stdlib.h>

namespace GreyDawn
{

	DirectoryMonitor::DirectoryMonitor(DirectoryMonitor&& other)
	{
		this->worker_ = std::move(other.worker_);
		this->notify_function_ = std::move(other.notify_function_);
        this->notify_queue_ = other.notify_queue_;
        other.notify_queue_ = -1;
        this->notify_watch_ = other.notify_watch_;
        other.notify_watch_ = -1;
        this->stop_signal_ = std::move(other.stop_signal_); 
	}

	DirectoryMonitor& DirectoryMonitor::operator=(DirectoryMonitor&& other)
	{
		if (this != std::addressof(other))
		{
            this->worker_ = std::move(other.worker_);
            this->notify_function_ = std::move(other.notify_function_);
            this->notify_queue_ = other.notify_queue_;
            other.notify_queue_ = -1;
            this->notify_watch_ = other.notify_watch_;
            other.notify_watch_ = -1;
            this->stop_signal_ = std::move(other.stop_signal_); 
		}
		return *this;
	}

	DirectoryMonitor::~DirectoryMonitor()
	{
		Stop();
	}

	bool DirectoryMonitor::Start(std::function<void()> notify_function, const std::string& path)
	{
        Stop();
        if (!stop_signal_.Initialize()) {
            return false;
        }
        stop_signal_.Clear();
        notify_function_ = std::move(notify_function);
        notify_queue_ = inotify_init();
        if (notify_queue_ < 0) {
            return false;
        }
        int flags = fcntl(notify_queue_, F_GETFL, 0);
        if (flags < 0) {
            (void)close(notify_queue_);
            notify_queue_ = -1;
            return false;
        }
        flags |= O_NONBLOCK;
        if (fcntl(notify_queue_, F_SETFL, flags) < 0) {
            (void)close(notify_queue_);
            notify_queue_ = -1;
            return false;
        }
        notify_watch_ = inotify_add_watch(
            notify_queue_,
            path.c_str(),
            IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO
        );
        if (notify_watch_ < 0) {
            (void)close(notify_queue_);
            notify_queue_ = -1;
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
        stop_signal_.Set();
        worker_.join();
        notify_watch_ = -1;
        if(close(notify_queue_) < 0)
            GD_LOG_OUTPUT_SYSTEM_ERROR();
        notify_queue_ = -1;
    }

	void DirectoryMonitor::Run()
	{
        try{
            const int stop_select_handle = (stop_signal_.pipe())[0];
            const int nfds = std::max(stop_select_handle, notify_queue_) + 1;
            fd_set readfds;
            std::vector< uint8_t > buffer(65536);
            for (;;) {
                FD_ZERO(&readfds);
                FD_SET(stop_select_handle, &readfds);
                FD_SET(notify_queue_, &readfds);
                (void)select(nfds, &readfds, NULL, NULL, NULL);
                if (FD_ISSET(stop_select_handle, &readfds)) {
                    break;
                }
                if (FD_ISSET(notify_queue_, &readfds)) {
                    while (read(notify_queue_, &buffer[0], buffer.size()) > 0) {
                    }
                    notify_function_();
                }
            }
        }catch(const std::exception& e){
            GD_LOG_ERROR("[std::exception>{}]", e.what());
        }catch(...){
            GD_LOG_ERROR("Unknow Exception");
        }
	}
}
















