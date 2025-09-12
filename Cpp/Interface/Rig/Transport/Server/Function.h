#pragma once
#include <Rig/Transport/Exception.h>
#include <Rig/Transport/IBuffer.h>
#include <functional>
#include <optional>

namespace Rig::Transport::Server
{
using FunctionResult = std::optional<Exception>;
using FunctionT = std::function<FunctionResult(IBuffer& data)>;
inline FunctionResult MissingFunction(IBuffer&)
{
    return Exception("Function not found");
}
}