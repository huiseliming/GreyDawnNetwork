#pragma once 
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>
#include "utility/Logger.h"

namespace GreyDawn
{
class PipeSignal
{
public:
    PipeSignal() = default;
    PipeSignal(const PipeSignal&) = delete;
    PipeSignal(PipeSignal&& other)
    {
        this->pipe_[0] = other.pipe_[0];
        other.pipe_[0] = -1;
        this->pipe_[1] = other.pipe_[1];
        other.pipe_[1] = -1;
    }
    PipeSignal& operator=(const PipeSignal&) = delete;
    PipeSignal& operator=(PipeSignal&& other)
    {
        if(this != std::addressof(other))
        {
            this->pipe_[0] = other.pipe_[0];
            other.pipe_[0] = -1;
            this->pipe_[1] = other.pipe_[1];
            other.pipe_[1] = -1;
        }
        return *this;
    }
    ~PipeSignal()
    {
        if (pipe_[1] >= 0) {
            (void)close(pipe_[1]);
        }
        if (pipe_[0] >= 0) {
            (void)close(pipe_[0]);
        }
    }
    bool Initialize()
    {
        if (pipe_[0] >= 0) {
            return true;
        }
        if (::pipe(pipe_) != 0) {
            GD_LOG_ERROR("[pipe errno>{:d} | strerror>{}]", errno, strerror(errno));
            pipe_[0] = -1;
            pipe_[1] = -1;
            return false;
        }
        for (int i = 0; i < 2; ++i) {
            int flags = fcntl(pipe_[i], F_GETFL, 0);
            if (flags < 0) {
                GD_LOG_ERROR("[fcntl errno>{:d} | strerror>{}]", errno, strerror(errno));
                if(close(pipe_[0]) < 0)
                    GD_LOG_ERROR("[close errno>{:d} | strerror>{}]", errno, strerror(errno));
                if(close(pipe_[1]) < 0)
                    GD_LOG_ERROR("[close errno>{:d} | strerror>{}]", errno, strerror(errno));
                pipe_[0] = -1;
                pipe_[1] = -1;
                return false;
            }
            flags |= O_NONBLOCK;
            if (fcntl(pipe_[i], F_SETFL, flags) < 0) {
                GD_LOG_ERROR("[fcntl errno>{:d} | strerror>{}]", errno, strerror(errno));
                if(close(pipe_[0]) < 0)
                    GD_LOG_ERROR("[close errno>{:d} | strerror>{}]", errno, strerror(errno));
                if(close(pipe_[1]) < 0)
                    GD_LOG_ERROR("[close errno>{:d} | strerror>{}]", errno, strerror(errno));
                pipe_[0] = -1;
                pipe_[1] = -1;
                return false;
            }
        }
        return true;
    }

    bool Set()
    {
        uint8_t token = '.';
        if(write(pipe_[1], &token, 1) < -1 ){
            GD_LOG_ERROR("[write errno>{:d} | strerror>{}]", errno, strerror(errno));
            return false;
        }
        return true;
    }

    bool Clear()
    {
        uint8_t token;
        if(read(pipe_[0], &token, 1) < -1){
            GD_LOG_ERROR("[write errno>{:d} | strerror>{}]", errno, strerror(errno));
            return false;
        }
        return true;
    }

    bool IsSet()
    {
        fd_set readfds;
        struct timeval timeout = {0};
        FD_ZERO(&readfds);
        FD_SET(pipe_[0], &readfds);
        return (select(pipe_[0] + 1, &readfds, NULL, NULL, &timeout) > 0);
    }

    int* pipe()
    {
        return &pipe_[0];
    }

private:
    int pipe_[2] = {-1, -1};
};
}




