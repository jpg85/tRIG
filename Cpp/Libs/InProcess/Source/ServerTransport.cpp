#include <Rig/Transport/InProcess/ServerTransport.h>
#include <rigtransport_inprocess_server.h>

namespace Rig::Transport::InProcess
{
std::shared_ptr<ServerTransport> ServerTransport::Create(std::string_view /*uri*/, std::span<const std::pair<std::string_view, Server::FunctionT>> functions)
{
    auto transport = std::make_shared<ServerTransport>();
    Rig::Transport::InProcess::RegisterFunctions(functions);
    return transport;
}
}