#pragma once
#include <asio.hpp>
#include <memory>
#include <deque>
#include "utility/Logger.h"

namespace GreyDawn
{
namespace Net
{
    class IPackage
    {
    public:
        virtual ~IPackage() = default;
        virtual size_t GetHeaderSize() = 0;
        virtual size_t GetBodySize(std::vector<uint8_t>& package) = 0;
        virtual uint8_t* Header(std::vector<uint8_t>& package) = 0;
        virtual uint8_t* Body(std::vector<uint8_t>& package) = 0;
    };

    class DefaultPackage : public IPackage
    {
    public:
        size_t GetHeaderSize() override
        {
            return sizeof(size_t);
        }
        size_t GetBodySize(std::vector<uint8_t>& package) override
        {
            return *(size_t*)package.data();
        }
        uint8_t* Header(std::vector<uint8_t>& package) override
        {
            return package.data();
        }
        uint8_t* Body(std::vector<uint8_t>& package) override
        {
            return package.data() + GetHeaderSize();
        }
    };

    class Session : public std::enable_shared_from_this<Session>, public DefaultPackage 
    {
    public:
        Session::Session(asio::io_service& io_service, asio::ip::tcp::socket socket, std::function<void(Session&, std::vector<uint8_t>)> package_handle)
            : io_service_(io_service)
            , strand_(io_service)
            , heartbeat_delay_timer_(io_service)
            , socket_(std::move(socket))
            , package_handle_(package_handle)
        {
        }
        virtual Session::~Session()
        {

        }
        Session::Session(const Session&& other) = delete;
        Session::Session(Session&& other) = delete;
        Session& Session::operator=(const Session&& other) = delete;
        Session& Session::operator=(Session&& other) = delete;

        virtual void Start()
        {
            ip_ = socket_.remote_endpoint().address().to_string();
            port_ = socket_.remote_endpoint().port();
            DoReadHeader();
        }

        virtual void ErrorHandle(const std::error_code& ec)
        {
            
        }
        
        void Shutdown()
        {
            io_service_.post([this] {socket_.close(); });
        }

        void SendPackage(std::vector<uint8_t> package)
        {
            strand_.post(
            [&, package = std::move(package)]() mutable
            {
                std::array<uint8_t, sizeof(size_t)> header;
                (*(size_t*)&header[0]) = package.size();
                package.insert(package.begin(), header.begin(), header.end());
                package_deque_.push_back(std::move(package));
                if (package_deque_.size() > 1)
                    return;
                this->DoWrite();
            });
        }

    protected:
        // read funciton
        virtual void DoReadHeader() 
        {
            auto self(shared_from_this());
            //asio::async_read call callback util full header
            asio::async_read(socket_,asio::buffer(&read_buffer_[0], GetHeaderSize()),
            [this, self](const std::error_code& ec, std::size_t length)
            {
                if (ec) {
                    ErrorHandle(ec);
                    return;
                }
                assert(GetHeaderSize() == length);
                package_.insert(package_.end(),read_buffer_.begin(), read_buffer_.begin() + length);
                remain_body_length_ = GetBodySize(package_);
                DoReadBody();
            });
        }

        virtual void DoReadBody() 
        {
            auto self(shared_from_this());
            asio::async_read(socket_, asio::buffer(read_buffer_, (remain_body_length_ < kReadBufferSize) ? remain_body_length_ : kReadBufferSize),
            [this, self](const std::error_code& ec, std::size_t length)
            {
                if (ec) {
                    ErrorHandle(ec);
                    return; 
                }
                assert(length <= kReadBufferSize);
                package_.insert(package_.end(), read_buffer_.begin(), read_buffer_.begin() + length);
                remain_body_length_ -= length;
                if (remain_body_length_ == 0) {
                    package_handle_(*this, std::move(package_));
                    DoReadHeader();
                } else {
                    DoReadBody();
                }
            });
        }

        //write funciton
        void DoWrite()
        {
            const std::vector<uint8_t>& bytes = package_deque_[0];
            asio::async_write(socket_, asio::buffer(bytes, bytes.size()), strand_.wrap(std::bind(&Session::WriteHandle, this, std::placeholders::_1, std::placeholders::_2)));
        }

        void WriteHandle(const std::error_code& ec, std::size_t length)
        {
            package_deque_.pop_front();
            if (ec)
            {
                GD_LOG_ERROR("session <{0}:{1:d}> : error_code = {2:d}, message = {3}", ip_, port_, ec.value(), ec.message());
                ErrorHandle(ec);
            }
            if (!package_deque_.empty())
            {
                this->DoWrite();
            }
        }

    public:
        static std::function<void(Session&, std::vector<uint8_t>)> default_package_handle;
        constexpr static size_t kReadBufferSize = 4096;
        constexpr static size_t kWriteBufferSize = 4096;
    protected:
        asio::io_service& io_service_;
        asio::io_service::strand strand_;
        asio::ip::tcp::socket socket_;

        std::string ip_;
        uint32_t port_;

        std::function<void(Session&, std::vector<uint8_t>)> package_handle_;

        asio::steady_timer heartbeat_delay_timer_;
        std::deque<std::vector<uint8_t>> package_deque_;

        std::size_t remain_body_length_ = 0;
        std::array<char, kReadBufferSize> read_buffer_;
        std::vector<uint8_t> package_;
    };
}
}



