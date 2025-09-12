#pragma once
#include <Rig/Transport/Client/ITransport.h>
#include <limits>
#include <memory>

namespace Rig::Transport::InProcess
{
class ClientTransport : public Client::ITransport
{
public:
    ~ClientTransport() override;
    static std::shared_ptr<ClientTransport> Create(std::string_view uri, std::span<const std::string_view> functionList);

    FunctionResult CallFunction(std::uint32_t functionId, std::unique_ptr<IBuffer> data) override;

private:
    void FindFunctions(std::span<const std::string_view> functionList);
    std::uint32_t m_apiIdx = std::numeric_limits<std::uint32_t>::max();
};
}