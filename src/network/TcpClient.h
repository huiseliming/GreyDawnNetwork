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
            std::shared_ptr<std::promise<std::shared_ptr<Session>>> promise_sptr(new std::promise<std::shared_ptr<Session>>());
            auto future = promise_sptr->get_future();
            std::shared_ptr<asio::ip::tcp::socket> socket_sptr(new asio::ip::tcp::socket(io_service_));
            asio::async_connect(*socket_sptr, endpoints,
            [this, promise_sptr, socket_sptr](std::error_code ec, asio::ip::tcp::endpoint)
            {
                if (ec)
                {
                    GD_LOG_ERROR("error_code = {0:d}\n message = {1:s}", ec.value(), ec.message());
                    promise_sptr->set_value(std::shared_ptr<Session>(nullptr));
                    return;
                }
                promise_sptr->set_value(std::make_shared<Session>(io_service_, std::move(*socket_sptr), Session::default_package_handle));
                //TODO: set the std::future<std::shared_ptr<Session>> 
            });
            return future;
        }
    private:
        asio::io_service& io_service_;
    };
}
}