#pragma once 
#include <asio.hpp>
#include "utility/Logger.h"


namespace GreyDawn
{
namespace Net
{
	template<typename Session>
	class Listener
	{
	public:
		Listener::Listener(asio::io_service& io_service, asio::ip::tcp::endpoint endpoint, std::string name = "Listener")
			:io_service_(io_service)
			, acceptor_(io_service, endpoint)
			, name_(name)
		{
		}
		Listener::~Listener()
		{
		}
		Listener(const Listener& other) = delete;
		Listener(Listener&& other) = delete;
		Listener& operator=(const Listener& other) = delete;
		Listener& operator=(Listener&& other) = delete;

	private:
		void DoAccept()
		{
			acceptor_.async_accept(
				[this](const std::error_code& ec, asio::ip::tcp::socket socket)
			{
				if (ec)
				{
					GD_LOG_ERROR("<{0} catch(asio::error_code)>\n error_code = {1:d}\n message = {2:s}", name_, ec.value(), ec.message());
					return;
				}
				std::string ip = socket.remote_endpoint().address().to_string();
				uint16_t port = socket.remote_endpoint().port();
				GD_LOG_INFO("<{0}:{1:d}> connected!", ip, port);
				std::string fullAddress = fmt::format("<{0:s}:{1:d}>", ip, port);
				std::make_shared<Session>(io_service_, std::move(socket))->Start();
				DoAccept();
			});
		}

		std::string name_;
		asio::io_service& io_service_;
		asio::ip::tcp::acceptor acceptor_;
	};
}

}



