#pragma once
#include <Rig/Transport/Server/Function.h>
#include <Rig/Transport/Server/ITransport.h>
#include <memory>
#include <span>

namespace Rig::Transport::InProcess
{
class ServerTransport : public Server::ITransport
{
public:
    ~ServerTransport() override = default;
    static std::shared_ptr<ServerTransport> Create(std::string_view uri, std::span<const std::pair<std::string_view, Server::FunctionT>> functions);
};
}