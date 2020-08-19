#pragma once
#include <asio.hpp>
#include <memory>
#include <deque>
#include "utility/Logger.h"





namespace GreyDawn
{
namespace Net
{
    struct Package 
    {
    public:
        Package() = default;
        Package(const Package& other) = default;
        Package(Package&& other)
        {
            header_ = std::move(other.header_);
            body_ = std::move(other.body_);
        }
        Package& operator=(const Package& other) = default;
        Package& operator=(Package&& other)
        {
            if (this != std::addressof(other))
            {
                header_ = std::move(other.header_);
                body_ = std::move(other.body_);
            }
            return *this;
        }

        size_t GetBodySize() 
        {
            return *(size_t*)header_.data();
        }

        std::vector<uint8_t> header_;
        std::vector<uint8_t> body_;
    };

    class Session : public std::enable_shared_from_this<Session>
    {
    public:
        Session::Session(asio::io_service& io_service, asio::ip::tcp::socket socket)
            : io_service_(io_service)
            , strand_(io_service)
            , heartbeat_delay_timer_(io_service)
            , socket_(std::move(socket))
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

    private:
        // read funciton
        virtual void DoReadHeader() 
        {
            auto self(shared_from_this());
            socket_.async_read_some(asio::buffer(&read_buffer_[0], remain_header_length_),
            [this, self](const std::error_code& ec, std::size_t length)
            {
                if (ec) {
                    ErrorHandle(ec);
                    return;
                }
                package_.header_.insert(
                    package_.header_.begin(),
                    read_buffer_.begin(),
                    read_buffer_.begin() + length
                );
                if (GetHeaderSize() > length) // 可能一次读取未读取到完整的header
                {
                    remain_header_length_ -= length;
                    if (remain_header_length_ == 0)
                    {
                        remain_header_length_ = GetHeaderSize();
                        remain_body_length_ = package_.GetBodySize();
                        DoReadBody();
                    }
                    else DoReadHeader();
                }
                else {
                    remain_body_length_ = package_.GetBodySize();
                    DoReadBody();
                }
            });
        }

        virtual void DoReadBody() 
        {
            auto self(shared_from_this());
            socket_.async_read_some(asio::buffer(read_buffer_, (remain_body_length_ < kReadBufferSize) ? remain_body_length_ : kReadBufferSize),
            [this, self](const std::error_code& ec, std::size_t length)
            {
                if (ec)
                {
                    ErrorHandle(ec);
                    return; 
                }

                package_.body_.insert(
                    package_.body_.begin(),
                    read_buffer_.begin(),
                    read_buffer_.begin() + length
                );
                remain_body_length_ -= length;

                if (remain_body_length_ == 0)
                {
                    package_handle_(std::move(package_));
                    assert(!package_.header_.size());
                    assert(!package_.body_.size());
                    DoReadHeader();
                }
                else DoReadBody();
            });
        }

        //write funciton
        void DoWrite()
        {
            const std::vector<uint8_t>& bytes = package_deque_[0];
            asio::async_write(socket_, asio::buffer(bytes, bytes.size()), strand_.wrap(std::bind(&Session::WriteHandle, this, std::placeholders::_1, std::placeholders::_2)));
        }

        void WriteHandle(const std::error_code& ec, std::size_t length) //��ɻص���ͬһ��strand����Ҫͬ��m_packageDeque�ķ���
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

        virtual size_t GetHeaderSize()
        {
            return kHeaderSize;
        }

    public:
        constexpr static size_t kReadBufferSize = 4096;
        constexpr static size_t kWriteBufferSize = 4096;
        constexpr static size_t kHeaderSize = sizeof(size_t);
    private:
        asio::io_service& io_service_;
        asio::io_service::strand strand_;
        asio::ip::tcp::socket socket_;

        std::string ip_;
        uint32_t port_;

        std::function<void(Package)> package_handle_;

        asio::steady_timer heartbeat_delay_timer_;
        std::deque<std::vector<uint8_t>> package_deque_;

        //std::deque<std::vector<uint8_t>> package_deque_;
        //std::vector<uint8_t> package_buffer_;
        // for read 
        std::size_t remain_body_length_ = 0;
        std::size_t remain_header_length_ = GetHeaderSize();
        std::array<char, kReadBufferSize> read_buffer_;
        Package package_;
    };
}
}



