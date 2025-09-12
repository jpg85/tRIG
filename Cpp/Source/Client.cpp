#include "TransportFactory.h"
#include "Rig/Transport/Client/ITransportFactory.h"
#include <Rig/Transport/Client/Transport.h>
#include <Rig/Transport/InProcess/ClientTransport.h>

namespace Rig::Transport::Client
{
using ClientTransportFactory = TransportFactory<ITransportFactory, ITransport, std::span<const std::string_view>>;
namespace
{
static ClientTransportFactory s_transportFactory = {
    {
        {std::string("inprocess"), (ClientTransportFactory::CreateFunctionT)&InProcess::ClientTransport::Create},
    }
};
}

ITransportFactory* GetTransportFactory()
{
    return &s_transportFactory;
}
}
