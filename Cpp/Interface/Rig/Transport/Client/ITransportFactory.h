#pragma once
#include "ITransport.h"
#include <memory>
#include <span>
#include <string_view>

namespace Rig::Transport::Client
{
struct ITransportFactory
{
    virtual ~ITransportFactory() = default;
    virtual std::shared_ptr<ITransport> CreateTransport(std::string_view uri, std::span<const std::string_view> functionList) = 0;
};
}