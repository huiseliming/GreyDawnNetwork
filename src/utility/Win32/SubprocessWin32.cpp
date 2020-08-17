#include "utility/Subprocess.h"
#include "utility/Logger.h"
#include "utility/SystemErrorException.h"
#include <array>
#include <signal.h>
#include <inttypes.h>
#include <Psapi.h>

namespace GreyDawn
{
    std::vector< char > MakeCommandLine(const std::string& program, const std::vector< std::string >& args) 
    {
        std::vector< char > commandLine;
        if (program.find_first_of('"') == std::string::npos) {
            commandLine.insert(commandLine.end(), program.begin(), program.end());
        }
        else {
            commandLine.push_back('"');
            commandLine.insert(commandLine.end(), program.begin(), program.end());
            commandLine.push_back('"');
        }
        for (const auto arg : args) {
            commandLine.push_back(' ');
            if (arg.find_first_of(" \t\n\v\"") == std::string::npos) {
                commandLine.insert(commandLine.end(), arg.begin(), arg.end());
            }
            else {
                commandLine.push_back('"');
                int slashCount = 0;
                for (int i = 0; i < (int)arg.length(); ++i) {
                    if (arg[i] == '\\') {
                        ++slashCount;
                    }
                    else {
                        commandLine.insert(commandLine.end(), slashCount, '\\');
                        if (arg[i] == '"') {
                            commandLine.insert(commandLine.end(), slashCount + 1, '\\');
                        }
                        commandLine.push_back(arg[i]);
                        slashCount = 0;
                    }
                }
                if (slashCount > 0) {
                    commandLine.insert(commandLine.end(), slashCount * 2, '\\');
                }
                commandLine.push_back('"');
            }
        }
        commandLine.push_back(0);
        return commandLine;
    }

    Subprocess::Subprocess()
    {
    }

    Subprocess::~Subprocess()
    {
        JoinChild();
        if (read_pipe_ != INVALID_HANDLE_VALUE) {
            uint8_t token = '.';
            DWORD amtWritten;
            (void)WriteFile(read_pipe_, &token, 1, &amtWritten, NULL);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            (void)CloseHandle(read_pipe_);
        }
    }

    Subprocess::Subprocess(Subprocess&& other)
    {
        worker_ = std::move(other.worker_);
        child_exited_ = std::move(other.child_exited_);
        child_crashed_ = std::move(other.child_crashed_);
        child_ = other.child_;
        other.child_ = INVALID_HANDLE_VALUE;
        read_pipe_ = other.read_pipe_;
        other.read_pipe_ = INVALID_HANDLE_VALUE;
        write_pipe_ = other.write_pipe_;
        other.write_pipe_ = INVALID_HANDLE_VALUE;
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
            other.child_ = INVALID_HANDLE_VALUE;
            read_pipe_ = other.read_pipe_;
            other.read_pipe_ = INVALID_HANDLE_VALUE;
            write_pipe_ = other.write_pipe_;
            other.write_pipe_ = INVALID_HANDLE_VALUE;
            previous_signal_handler_ = other.previous_signal_handler_;
            other.previous_signal_handler_ = nullptr;
        }
        return *this;
    }

    unsigned int Subprocess::Create(
        std::string program,
        const std::vector< std::string >& args,
        std::function< void() > child_exited,
        std::function< void() > child_crashed
    ) {
        HANDLE child_read_pipe;
        HANDLE child_write_pipe;
        try
        {
            SECURITY_ATTRIBUTES sa;
            sa.nLength = sizeof(sa);
            sa.bInheritHandle = TRUE;
            sa.lpSecurityDescriptor = NULL;
            THROW_SYSTEM_ERROR_IF_FAILED(!CreatePipe(&read_pipe_, &child_write_pipe, &sa, 0));
            THROW_SYSTEM_ERROR_IF_FAILED(!CreatePipe(&child_read_pipe, &write_pipe_, &sa, 0));

            std::vector< std::string > command_line_args;
            command_line_args.push_back("child"); //child mark
            command_line_args.push_back(fmt::format("{:d}", (uint64_t)child_read_pipe));
            command_line_args.push_back(fmt::format("{:d}", (uint64_t)child_write_pipe));
            command_line_args.insert(command_line_args.end(), args.begin(), args.end());
            if (
                (program.length() < 4)
                || (program.substr(program.length() - 4) != ".exe")
                ) {
                program += ".exe";
            }
            auto commandLine = MakeCommandLine(program, command_line_args);

            // Launch program.
            STARTUPINFOA si;
            (void)memset(&si, 0, sizeof(si));
            si.cb = sizeof(si);
            PROCESS_INFORMATION pi;
            THROW_SYSTEM_ERROR_IF_FAILED(
                !CreateProcessA(
                    program.c_str(),
                    &commandLine[0],
                    NULL,
                    NULL,
                    TRUE,
                    0,
                    NULL,
                    NULL,
                    &si,
                    &pi
                )
            );
            child_ = pi.hProcess;
            child_crashed_ = child_crashed;
            child_exited_ = child_exited;
            (void)CloseHandle(child_read_pipe);
            (void)CloseHandle(child_write_pipe);
            worker_ = std::thread(&Subprocess::PipeMessage, this);
            return (unsigned int)pi.dwProcessId;
        }
        catch (const std::exception& e)
        {
            GD_LOG_ERROR("[std::exception>{}]",e.what());
            if (read_pipe_)
                (void)CloseHandle(read_pipe_);
            if (write_pipe_)
                (void)CloseHandle(write_pipe_);
            if (child_read_pipe)
                (void)CloseHandle(child_read_pipe);
            if (child_write_pipe)
                (void)CloseHandle(child_write_pipe);
            return 0;
        }
        catch (...)
        {
            GD_LOG_ERROR("Unkonw Expection");
            return 0;
        }
    }

    void Subprocess::PipeMessage() 
    {
        // message must 64 bit alignment otherwise the head will be split
        std::vector<uint8_t> message;
        DWORD amtRead = 0;
        BOOL return_code;
        uint64_t message_size = 1;
        uint64_t remain_size = 0;
        for (;;)
        {
            std::array<uint8_t, 4096> buffer;
            memset(&buffer[0],0xFF,4096);
            return_code = ReadFile(read_pipe_, &buffer, 4096, &amtRead, NULL);
            if (return_code == 0)
            {
                if(child_ != INVALID_HANDLE_VALUE)
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
        if (child_ != INVALID_HANDLE_VALUE) 
        {
            if(message_size == 0)
                child_exited_();
            (void)WaitForSingleObject(child_, INFINITE);
        }
        else
        {
            std::lock_guard<std::mutex> lock(write_pipe_mutex_);
            uint64_t signal = 0;
            DWORD byte_written;
            (void)WriteFile(write_pipe_, &signal, sizeof(uint64_t), &byte_written, NULL);
        }
    }

    void Subprocess::JoinChild() {
        if (worker_.joinable()) {
            {
                std::lock_guard<std::mutex> lock(write_pipe_mutex_);
                uint64_t signal = 0;
                DWORD byte_written;
                (void)WriteFile(write_pipe_, &signal, sizeof(uint64_t), &byte_written, NULL);
            }
            worker_.join();
            (void)CloseHandle(child_);
            child_ = INVALID_HANDLE_VALUE;
            (void)CloseHandle(read_pipe_);
            read_pipe_ = INVALID_HANDLE_VALUE;
        }
    }

    bool Subprocess::ContactParent(std::vector< std::string >& args) {
        if (
            (args.size() >= 3)
            && (args[0] == "child")
            ) {
            uint64_t pipeNumber;
            if (sscanf(args[1].c_str(), "%" SCNu64, &pipeNumber) != 1) {
                return false;
            }
            read_pipe_ = (HANDLE)pipeNumber;
            if (sscanf(args[2].c_str(), "%" SCNu64, &pipeNumber) != 1) {
                return false;
            }
            write_pipe_ = (HANDLE)pipeNumber;
            args.erase(args.begin(), args.begin() + 3);
            return true;
        }
        return false;
    }

    unsigned int Subprocess::GetCurrentProcessId() {
        return (unsigned int)::GetCurrentProcessId();
    }

    void Subprocess::Kill(unsigned int id) {
        const auto processHandle = OpenProcess((PROCESS_TERMINATE), FALSE, (DWORD)id);
        if (processHandle == NULL) {
            GD_LOG_OUTPUT_SYSTEM_ERROR();
            return;
        }
        if(!TerminateProcess(processHandle, 255))
            GD_LOG_OUTPUT_SYSTEM_ERROR();
        if (!CloseHandle(processHandle))
            GD_LOG_OUTPUT_SYSTEM_ERROR();
    }



}































