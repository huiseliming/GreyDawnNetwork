#include <gtest/gtest.h>
#include <network/TcpClient.h>
#include <network/Listener.h>
#include <utility/Utility.h>
using namespace GreyDawn;

static std::promise<std::string> promise;

void handle(Net::Session& session, std::vector<uint8_t> package)
{
	package.push_back('\0');
	promise.set_value((char*)session.Body(package));
}

TEST(tests, ConnectionTest)
{
	asio::io_service io_service;
	Net::Listener listener(io_service,asio::ip::tcp::endpoint(asio::ip::address_v4(),58080), handle);

	asio::ip::tcp::resolver resolver(io_service);
	Net::TcpClient tcp_client(io_service);
	std::thread t([&]{
		io_service.run();
	});
	auto endpoints = resolver.resolve("localhost", "58080");
	auto client_futrue = tcp_client.DoConnect(endpoints);
	auto client_session = client_futrue.get();
	ASSERT_TRUE(client_session != nullptr);
	std::string hello_world = "hello world!";
	auto future = promise.get_future();
	client_session->SendPackage(std::vector<uint8_t>(hello_world.begin(), hello_world.end()));
	auto futrue_get = future.get();
	ASSERT_EQ(futrue_get, hello_world);
	client_session->Shutdown();
	listener.Stop();
	t.join();
}











