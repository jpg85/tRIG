#pragma once
#include <Rig/Transport/Exception.h>
#include <Rig/Transport/IBuffer.h>

#include <cstdint>
#include <memory>
#include <variant>

namespace Rig::Transport::Client
{
struct ITransport
{
    virtual ~ITransport() = default;
    using FunctionResult = std::variant<std::unique_ptr<IBuffer>, Exception>;
    virtual FunctionResult CallFunction(std::uint32_t functionId, std::unique_ptr<IBuffer> data) = 0;
};
}