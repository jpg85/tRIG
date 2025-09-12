#include "TransportFactory.h"
#include "Rig/Transport/Server/ITransportFactory.h"
#include <Rig/Transport/Server/Transport.h>
#include <Rig/Transport/InProcess/ServerTransport.h>

namespace Rig::Transport::Server
{
using ServerTransportFactory = TransportFactory<ITransportFactory, ITransport, std::span<const std::pair<std::string_view, FunctionT>>>;
namespace
{
static ServerTransportFactory s_transportFactory = {
    {
        {std::string("inprocess"), (ServerTransportFactory::CreateFunctionT)&InProcess::ServerTransport::Create},
    }
};
}

ITransportFactory* GetTransportFactory()
{
    return &s_transportFactory;
}
}
