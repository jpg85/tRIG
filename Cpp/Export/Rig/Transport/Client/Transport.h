#pragma once
#include "rigtransportcpp_export.h"
#include <Rig/Transport/Client/ITransportFactory.h>

namespace Rig::Transport::Client
{
RIGTRANSPORTCPP_EXPORT ITransportFactory* GetTransportFactory();
}