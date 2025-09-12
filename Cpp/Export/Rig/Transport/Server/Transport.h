#pragma once
#include "rigtransportcpp_export.h"
#include <Rig/Transport/Server/ITransportFactory.h>

namespace Rig::Transport::Server
{
RIGTRANSPORTCPP_EXPORT ITransportFactory* GetTransportFactory();
}