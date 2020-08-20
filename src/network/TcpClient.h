#pragma once
#include <asio.hpp>
#include <memory>
#include "TcpClient.h"
#include "Session.h"

namespace GreyDawn
{
namespace Net
{
    


    class TcpClient
    {
    public:
        TcpClient(asio::io_service& io_service)
            : io_service_(io_service)
        {
        }
        std::future<std::shared_ptr<Session>> DoConnect(const asio::ip::tcp::resolver::results_type& endpoints)
        {
            asio::ip::tcp::socket socket(io_service_);
            asio::async_connect(socket, endpoints,
            [this](std::error_code ec, asio::ip::tcp::endpoint)
            {
                if (!ec)
                {
                    GD_LOG_ERROR("error_code = {1:d}\n message = {2:s}", ec.value(), ec.message());
                    return;
                }
                //TODO: set the std::future<std::shared_ptr<Session>> 

            });
        }
    private:
        asio::io_service& io_service_;
    };
}
}