#include <gtest/gtest.h>
#include <network/TcpClient.h>
#include <network/Listener.h>
#include <utility/Utility.h>
using namespace GreyDawn;




TEST(tests, ConnectionTest)
{
	asio::io_service io_service;
	Net::Listener listener(io_service,asio::ip::tcp::endpoint(asio::ip::address_v4(),58080));

	asio::ip::tcp::resolver resolver(io_service);
	auto endpoints = resolver.resolve("localhost", "58080");
	Net::TcpClient tcp_client(io_service);
	std::string str = "hello world!";
	//tcp_client.SendPackage(std::vector<uint8_t>(str.begin(),str.end()));
	io_service.run();
}











