#include "utility/Subprocess.h"
#include "utility/Logger.h"
#include "utility/SystemErrorException.h"
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
        HANDLE read_pipe_;
        HANDLE write_pipe_;
        HANDLE child_read_pipe;
        HANDLE child_write_pipe;
        try
        {
            SECURITY_ATTRIBUTES sa;
            (void)memset(&sa, 0, sizeof(sa));
            sa.nLength = sizeof(sa);
            sa.bInheritHandle = TRUE;
            THROW_SYSTEM_ERROR_IF_FAILED(!CreatePipe(&read_pipe_, &child_write_pipe, &sa, 0));
            THROW_SYSTEM_ERROR_IF_FAILED(!CreatePipe(&child_read_pipe, &write_pipe_, &sa, 0));

            std::vector< std::string > command_line_args;
            command_line_args.push_back("child"); //child mark
            command_line_args.push_back(fmt::format("{:x}", (uint64_t)child_read_pipe));
            command_line_args.push_back(fmt::format("{:x}", (uint64_t)child_write_pipe));
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
            worker_ = std::thread(&Subprocess::MonitorChild, this);
            return (unsigned int)pi.dwProcessId;
        }
        catch (const std::exception& e)
        {
            GD_LOG_ERROR("[std::exception>{}]",e.what());
            if (read_pipe_)
                CloseHandle(read_pipe_);
            if (write_pipe_)
                CloseHandle(write_pipe_);
            if (child_read_pipe)
                CloseHandle(child_read_pipe);
            if (child_write_pipe)
                CloseHandle(child_write_pipe);
            return 0;
        }
        catch (...)
        {
            GD_LOG_ERROR("Unkonw Expection");
            return 0;
        }
    }

    void SignalHandler(int)
    {

    }

    void Subprocess::MonitorChild() {
        previous_signal_handler_ = signal(SIGINT, SignalHandler);
        (void)WaitForSingleObject(read_pipe_, INFINITE);
        uint8_t token;
        DWORD amtRead = 0;
        if (ReadFile(read_pipe_, &token, 1, &amtRead, NULL) == FALSE) {
            child_crashed_();
        }
        else {
            child_exited_();
        }
        (void)WaitForSingleObject(child_, INFINITE);
        (void)signal(SIGINT, previous_signal_handler_);
    }

    void Subprocess::JoinChild() {
        if (worker_.joinable()) {
            worker_.join();
            (void)CloseHandle(child_);
            child_ = INVALID_HANDLE_VALUE;
            (void)CloseHandle(read_pipe_);
            read_pipe_ = INVALID_HANDLE_VALUE;
        }
    }

    unsigned int Subprocess::StartChild(
        std::string program,
        const std::vector< std::string >& args,
        std::function< void() > child_exited,
        std::function< void() > child_crashed
    ){
        JoinChild();
        child_exited_ = child_exited;
        child_crashed_ = child_crashed;
        SECURITY_ATTRIBUTES sa;
        (void)memset(&sa, 0, sizeof(sa));
        sa.nLength = sizeof(sa);
        sa.bInheritHandle = TRUE;
        HANDLE write_pipe_;
        if (CreatePipe(&read_pipe_, &write_pipe_, &sa, 0) == FALSE) {
            return 0;
        }

        if (
            (program.length() < 4)
            || (program.substr(program.length() - 4) != ".exe")
            ) {
            program += ".exe";
        }

        std::vector< std::string > childArgs;
        childArgs.push_back("child");
        childArgs.push_back(fmt::format("{:x}", (uint64_t)write_pipe_));
        childArgs.insert(childArgs.end(), args.begin(), args.end());

        auto commandLine = MakeCommandLine(program, childArgs);

        // Launch program.
        STARTUPINFOA si;
        (void)memset(&si, 0, sizeof(si));
        si.cb = sizeof(si);
        PROCESS_INFORMATION pi;
        if (
            CreateProcessA(
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
                ) == 0
            ) {
            if(CloseHandle(read_pipe_))
                GD_LOG_OUTPUT_SYSTEM_ERROR();
            read_pipe_ = INVALID_HANDLE_VALUE;
            if (CloseHandle(write_pipe_))
                GD_LOG_OUTPUT_SYSTEM_ERROR();
            return 0;
        }
        child_ = pi.hProcess;
        if (CloseHandle(write_pipe_))
            GD_LOG_OUTPUT_SYSTEM_ERROR();
        worker_ = std::thread(&Subprocess::MonitorChild, this);
        return (unsigned int)pi.dwProcessId;
    }

    bool Subprocess::ContactParent(std::vector< std::string >& args) {
        if (
            (args.size() >= 2)
            && (args[0] == "child")
            ) {
            uint64_t pipeNumber;
            if (sscanf(args[1].c_str(), "%" SCNu64, &pipeNumber) != 1) {
                return false;
            }
            write_pipe_ = (HANDLE)pipeNumber;
            args.erase(args.begin(), args.begin() + 2);
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































