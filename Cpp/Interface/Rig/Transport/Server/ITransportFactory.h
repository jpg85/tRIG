#pragma once
#include "Function.h"
#include "ITransport.h"
#include <memory>
#include <string_view>

namespace Rig::Transport::Server
{
struct ITransportFactory
{
    virtual ~ITransportFactory() = default;
    virtual std::shared_ptr<ITransport> CreateTransport(std::string_view uri, std::span<const std::pair<std::string_view, FunctionT>> functions) = 0;
};
}