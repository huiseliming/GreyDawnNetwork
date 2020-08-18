#include "utility/Subprocess.h"
#include "utility/Logger.h"
#include "utility/SystemErrorException.h"
#include <limits.h>
#include <unistd.h>
#include <filesystem>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

namespace GreyDawn
{

    void CloseFilesByFilter(std::vector<int> keep_opens)
    {
        std::vector< std::string > fds;
        const std::string fdsDir("/proc/self/fd/");
        for(auto& fd :std::filesystem::directory_iterator("/proc/self/fd"))
        {
            const auto fdNumString = fd.path().string().substr(fdsDir.length());
            int fdNum;
            for(auto keep_open : keep_opens)
            {
                if (
                    (sscanf(fdNumString.c_str(), "%d", &fdNum) == 1)
                    && (fdNum != keep_open)
                ) {
                    (void)close(fdNum);
                }
            }
        }
    }

    Subprocess::Subprocess(){
    }
    Subprocess::Subprocess(Subprocess&& other)
    {
        worker_ = std::move(other.worker_);
        child_exited_ = std::move(other.child_exited_);
        child_crashed_ = std::move(other.child_crashed_);
        child_ = other.child_;
        other.child_ = -1;
        read_pipe_ = other.read_pipe_;
        other.read_pipe_ = -1;
        write_pipe_ = other.write_pipe_;
        other.write_pipe_ = -1;
        previous_signal_handler_ = other.previous_signal_handler_;
        other.previous_signal_handler_ = nullptr;
    }

    Subprocess& Subprocess::operator=(Subprocess&& other)
    {
        if(this != std::addressof(other))
        {
            worker_ = std::move(other.worker_);
            child_exited_ = std::move(other.child_exited_);
            child_crashed_ = std::move(other.child_crashed_);
            child_ = other.child_;
            other.child_ = -1;
            read_pipe_ = other.read_pipe_;
            other.read_pipe_ = -1;
            write_pipe_ = other.write_pipe_;
            other.write_pipe_ = -1;
            previous_signal_handler_ = other.previous_signal_handler_;
            other.previous_signal_handler_ = nullptr;
        }
        return *this;
    }

    Subprocess::~Subprocess() noexcept {
        JoinChild();
    }

    void Subprocess::JoinChild() {
        if (worker_.joinable()) {
            {
                std::lock_guard<std::mutex> lock(write_pipe_mutex_);
                uint64_t signal = 0;
                ssize_t byte_written;
                byte_written = write(write_pipe_, &signal, sizeof(uint64_t));
            }
            worker_.join();
            child_ = -1;
            (void)close(read_pipe_);
            read_pipe_ = -1;
            (void)close(write_pipe_);
            write_pipe_ = -1;
        }
    }

    void Subprocess::PipeMessage() 
    {
        // message must 64 bit alignment otherwise the head will be split
        std::vector<uint8_t> message;
        ssize_t amtRead = 0;
        uint64_t message_size = 1;
        uint64_t remain_size = 0;
        for (;;)
        {
            std::array<uint8_t, 4096> buffer;
            memset(&buffer[0],0xFF,4096);
            amtRead = read(read_pipe_, &buffer, 4096);
            if (amtRead == 0 || (amtRead < 0 && (errno != EINTR)))
            {
                if(child_ != -1)
                    child_crashed_();
                break;
            }
            if (remain_size == 0)
            {
                message.insert(message.begin(),buffer.begin(),buffer.begin() + sizeof(uint64_t));
                message_size = (*(uint64_t*)message.data());
                if (message_size == 0)
                    break;
                remain_size = message_size;
                message.insert(message.end(), buffer.begin() + sizeof(uint64_t), buffer.begin() + amtRead - sizeof(uint64_t));
                remain_size -= amtRead;
            }
            else
            {
                message.insert(message.end(), buffer.begin(), buffer.begin() + amtRead);
                remain_size -= amtRead;
            }
            if (remain_size = 0)
            {  
                message.push_back('\0');
                GD_LOG_INFO("[message : {}]", (char *)message.data());
                message.clear();
            }
        }
        if (child_ != -1) 
        {
            if(message_size == 0)
                child_exited_();
            (void)waitpid(child_, NULL, 0);
        }
        else
        {
            std::lock_guard<std::mutex> lock(write_pipe_mutex_);
            uint64_t signal = 0;
            ssize_t byte_written = 0;
            byte_written = write(write_pipe_, &signal, sizeof(uint64_t));
        }
    }

    unsigned int Subprocess::Create(
        std::string program,
        const std::vector< std::string >& args,
        std::function< void() > childExited,
        std::function< void() > childCrashed
    ) {
        JoinChild();
        childExited = childExited;
        childCrashed = childCrashed;
        int read_pipe[2];
        int write_pipe[2];
        THROW_SYSTEM_ERROR_IF_FAILED(pipe(read_pipe) < 0);
        THROW_SYSTEM_ERROR_IF_FAILED(pipe(write_pipe) < 0);
        static const std::string child_string = "child";
        std::vector< std::vector< char > > childArgs;
        childArgs.emplace_back(program.begin(),program.end());
        childArgs.emplace_back(child_string.begin(),child_string.end());
        const std::string child_read_pipe_num = fmt::format("{:d}",read_pipe[0]);
        const std::string child_write_pipe_num = fmt::format("{:d}",write_pipe[1]);
        childArgs.emplace_back(child_read_pipe_num.begin(),child_read_pipe_num.end());
        childArgs.emplace_back(child_write_pipe_num.begin(),child_write_pipe_num.end());
        for (const auto arg: args) {
            childArgs.emplace_back(arg.begin(),arg.end());
        }

        // Launch program.
        child_ = fork();
        if (child_ == 0) {
            CloseFilesByFilter(std::vector<int>({read_pipe[0],write_pipe[1]}));
            std::vector< char* > argv(childArgs.size() + 1);
            for (size_t i = 0; i < childArgs.size(); ++i) {
                argv[i] = &childArgs[i][0];
            }
            argv[childArgs.size()] = NULL;
            (void)execv(program.c_str(), &argv[0]);
            (void)exit(-1);
        } else if (child_ < 0) { 
            (void)close(read_pipe[0]);
            (void)close(read_pipe[1]);
            (void)close(write_pipe[0]);
            (void)close(write_pipe[1]);
            return 0;
        }
        read_pipe_ = write_pipe[0];
        (void)close(write_pipe[1]);
        write_pipe_ = read_pipe[1];
        (void)close(read_pipe[0]);
        worker_ = std::thread(&Subprocess::PipeMessage, this);
        return (unsigned int)child_;
    }

    bool Subprocess::ContactParent(std::vector< std::string >& args) {
        if (
            (args.size() >= 3)
            && (args[0] == "child")
            ) {
            int pipeNumber;
            if (sscanf(args[1].c_str(), "%d", &pipeNumber) != 1) {
                return false;
            }
            read_pipe_ = pipeNumber;
            if (sscanf(args[2].c_str(), "%d", &pipeNumber) != 1) {
                return false;
            }
            write_pipe_ = pipeNumber;
            args.erase(args.begin(), args.begin() + 3);
            return true;
        }
        return false;
    }

    unsigned int Subprocess::GetCurrentProcessId() {
        return (unsigned int)getpid();
    }

    void Subprocess::Kill(unsigned int id) {
        (void)kill(id, SIGKILL);
    }
}






























