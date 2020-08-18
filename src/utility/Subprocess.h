#pragma once
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#ifdef _WIN32
#include <Windows.h>
#elif __linux__

#endif

namespace GreyDawn
{

struct ProcessInfo {
    unsigned int id;

    std::string image;

};

class Subprocess 
{
public:

public:
    Subprocess();
    ~Subprocess();
    Subprocess(const Subprocess&) = delete;
    Subprocess(Subprocess&&);
    Subprocess& operator=(const Subprocess&) = delete;
    Subprocess& operator=(Subprocess&&);

    unsigned int Create(
        std::string program,
        const std::vector< std::string >& args,
        std::function< void() > child_exited,
        std::function< void() > child_crashed
    );


    static void SignalHandler(int);

    void PipeMessage();

    void JoinChild();

    static unsigned int StartDetached(
        std::string program,
        const std::vector< std::string >& args
        );

    bool ContactParent(std::vector< std::string >& args);

    static unsigned int GetCurrentProcessId();

    static std::vector< ProcessInfo > GetProcessList();

    static void Kill(unsigned int id);

private:
    std::thread worker_;
    std::function<void()> child_exited_;
    std::function<void()> child_crashed_;
#ifdef _WIN32
    HANDLE child_ = INVALID_HANDLE_VALUE;
    HANDLE read_pipe_ = INVALID_HANDLE_VALUE;
    HANDLE write_pipe_ = INVALID_HANDLE_VALUE;
    std::mutex write_pipe_mutex_;
    void (*previous_signal_handler_)(int) = nullptr;
#elif __linux__
    pid_t child_ = -1;
    int read_pipe_ = -1;
    int write_pipe_ = -1;
    std::mutex write_pipe_mutex_;
    void (*previous_signal_handler_)(int) = nullptr;
#endif

};

}




















