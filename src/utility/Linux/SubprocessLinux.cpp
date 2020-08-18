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

    std::vector< char > VectorFromVector(const std::string& s) {
        std::vector< char > v(s.length() + 1);
        for (size_t i = 0; i < s.length(); ++i) {
            v[i] = s[i];
        }
        v[s.length()] = '\0';
        return v;
    }

    void CloseFilesByFilter(std::vector<int> keep_opens)
    {
        std::vector< std::string > fds;
        const std::string fds_dir("/proc/self/fd/");
        for(auto& fd :std::filesystem::directory_iterator("/proc/self/fd"))
        {
            const auto fd_num_string = fd.path().string().substr(fds_dir.length());
            int fd_num;
            for(auto keep_open : keep_opens)
            {
                if (
                    (sscanf(fd_num_string.c_str(), "%d", &fd_num) == 1)
                    && (fd_num != keep_open)
                ) {
                    (void)close(fd_num);
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
                uint64_t token = 0;
                ssize_t byte_written;
                byte_written = write(write_pipe_, &token, sizeof(uint64_t));
            }
            worker_.join();
            child_ = -1;
            (void)close(read_pipe_);
            read_pipe_ = -1;
            (void)close(write_pipe_);
            write_pipe_ = -1;
        }
    }
    void SignalHandler(int){}

    void Subprocess::PipeMessage() 
    {
        //previous_signal_handler_ = signal(SIGINT, SignalHandler);
        // message must 64 bit alignment otherwise the head will be split
        std::vector<uint8_t> message;
        ssize_t read_bytes = 0;
        uint64_t message_size = 1;
        uint64_t remain_size = 0;
        for (;;)
        {
            std::array<uint8_t, 4096> buffer;
            memset(&buffer[0],0xFF,4096);
            read_bytes = read(read_pipe_, &buffer, 4096);
            if (read_bytes == 0 || (read_bytes < 0 && (errno != EINTR)))
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
                message.insert(message.end(), buffer.begin() + sizeof(uint64_t), buffer.begin() + read_bytes - sizeof(uint64_t));
                remain_size -= read_bytes;
            }
            else
            {
                message.insert(message.end(), buffer.begin(), buffer.begin() + read_bytes);
                remain_size -= read_bytes;
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
            uint64_t token = 0;
            ssize_t byte_written = 0;
            byte_written = write(write_pipe_, &token, sizeof(uint64_t));
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
        //(void)signal(SIGINT, previous_signal_handler_);
    }

    unsigned int Subprocess::Create(
        std::string program,
        const std::vector< std::string >& args,
        std::function< void() > child_exited,
        std::function< void() > child_crashed
    ) {
        JoinChild();
        child_exited_ = child_exited;
        child_crashed_ = child_crashed;
        int read_pipe[2];
        int write_pipe[2];
        THROW_SYSTEM_ERROR_IF_FAILED(pipe(read_pipe) < 0);
        THROW_SYSTEM_ERROR_IF_FAILED(pipe(write_pipe) < 0);
        std::vector< std::vector< char > > command_line_args;
        command_line_args.emplace_back(VectorFromVector(program));
        command_line_args.emplace_back(VectorFromVector("child"));
        command_line_args.emplace_back(VectorFromVector(fmt::format("{:d}",read_pipe[0])));
        command_line_args.emplace_back(VectorFromVector(fmt::format("{:d}",write_pipe[1])));
        for (const auto arg: args) {
            command_line_args.emplace_back(arg.begin(),arg.end());
        }

        // Launch program.
        child_ = fork();
        if (child_ == 0) {
            //CloseFilesByFilter(std::vector<int>({read_pipe[0],write_pipe[1]}));
            std::vector< char* > argv(command_line_args.size() + 1);
            for (size_t i = 0; i < command_line_args.size(); ++i) {
                argv[i] = &command_line_args[i][0];
            }
            argv[command_line_args.size()] = NULL;
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
            int pipe_number;
            if (sscanf(args[1].c_str(), "%d", &pipe_number) != 1) {
                return false;
            }
            read_pipe_ = pipe_number;
            if (sscanf(args[2].c_str(), "%d", &pipe_number) != 1) {
                return false;
            }
            write_pipe_ = pipe_number;
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






























