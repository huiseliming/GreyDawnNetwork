#include "Session.h"
namespace GreyDawn
{
namespace Net
{
std::function<void(Session&, std::vector<uint8_t>)> Session::default_package_handle = [](class Session& session, std::vector<uint8_t> package)
{
    package.push_back('\0');
    GD_LOG_INFO("message {}", (char*)session.Body(package));
};
}
}