#pragma once
#include <Rig/Transport/Exception.h>

#include <string_view>
#include <map>
#include <memory>

namespace Rig::Transport
{
template<typename FactoryT, typename TransportT, typename FunctionListT>
class TransportFactory : public FactoryT
{
public:
    using CreateFunctionT = std::shared_ptr<TransportT>(*)(std::string_view, FunctionListT);
    using TransportFactoriesMap = std::map<std::string, CreateFunctionT, std::less<>>;
    TransportFactory(TransportFactoriesMap factories)
        : m_transportFactories(std::move(factories))
    {
    }
    ~TransportFactory() override = default;

    std::shared_ptr<TransportT> CreateTransport(std::string_view uri, FunctionListT functions) override
    {
        //First, find the prefix to the URI
        const auto pos = uri.find("://");
        if(pos == std::string_view::npos)
        {
            throw Exception("Invalid URI, missing scheme");
        }
        //Then use that prefix to find the transport factory
        const auto scheme = uri.substr(0, pos);
        const auto it = m_transportFactories.find(scheme);
        if(it == m_transportFactories.end())
        {
            throw Exception("No transport registered for scheme: " + std::string(scheme));
        }
        //Finally, get the suffix to the URI and create the transport
        const auto suffix = uri.substr(pos + 3);
        return it->second(suffix, functions);
    }
private:
    TransportFactoriesMap m_transportFactories;
};
}