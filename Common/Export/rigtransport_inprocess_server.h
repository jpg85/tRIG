#pragma once
#include "rigtransportcommon_export.h"
#include <Rig/Transport/Server/Function.h>
#include <span>

namespace Rig::Transport::InProcess
{
RIGTRANSPORTCOMMON_EXPORT void RegisterFunctions(std::span<const std::pair<std::string_view, Server::FunctionT>> functions);
}