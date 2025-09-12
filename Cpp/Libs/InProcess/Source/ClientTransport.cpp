#include "Rig/Transport/Client/ITransport.h"
#include "Rig/Transport/IBuffer.h"
#include <Rig/Transport/InProcess/ClientTransport.h>
#include <cstdint>
#include <rigtransport_inprocess.h>

#include <cstring>
#include <vector>

namespace Rig::Transport::InProcess
{
namespace
{
struct InProcBuffer : IBuffer
{
public:
    InProcBuffer(uint8_t* data, uint32_t size)
        : m_data(data), m_size(size), m_originalSize(size)
    {
    }
    ~InProcBuffer() override
    {
        rigtransport_inprocess_free(m_data);
    }

    std::uint8_t* Data() override { return m_data; }
    std::uint32_t Size() const override { return m_size; }
    std::uint32_t MaxSize() const override { return m_originalSize; }
    void Resize(uint32_t size) override
    {
        if(size <= m_originalSize)
        {
            m_size = size;
        }
    }
private:
    uint8_t* m_data;
    uint32_t m_size;
    uint32_t m_originalSize;
};
}

std::shared_ptr<ClientTransport> ClientTransport::Create(std::string_view /*uri*/, std::span<const std::string_view> functionList)
{
    auto transport = std::make_shared<ClientTransport>();
    transport->FindFunctions(functionList);
    return transport;
}

ClientTransport::~ClientTransport()
{
    rigtransport_inprocess_releasefunctions(m_apiIdx);
}

void ClientTransport::FindFunctions(std::span<const std::string_view> functionList)
{
    std::vector<const char*> functionListPointers;
    functionListPointers.reserve(functionList.size());
    for (const auto& func : functionList)
    {
        functionListPointers.push_back(func.data());
    }
    uint32_t apiIdx = 0;
    const auto result = rigtransport_inprocess_findfunctions(functionListPointers.data(), static_cast<uint32_t>(functionListPointers.size()), &apiIdx);
    if(result != 0)
    {
        throw Exception(rigtransport_inprocess_geterrorstring(result));
    }
    m_apiIdx = apiIdx;
}

Client::ITransport::FunctionResult ClientTransport::CallFunction(std::uint32_t functionId, std::unique_ptr<IBuffer> data)
{
    uint8_t* outData = nullptr;
    uint32_t outDataSize = 0;
    const auto result = rigtransport_inprocess_callfunction(m_apiIdx, functionId, data->Data(), data->Size(), &outData, &outDataSize);
    if(result > 0)
    {
        std::string errorMessage(rigtransport_inprocess_geterrorstring(result));
        return Exception(errorMessage);
    }
    //If we get here, we need to see if we need to copy data back to the input buffer
    auto retData = std::move(data);
    if(outDataSize > retData->Size())
    {
        //If the output data is larger, that means callfunction allocated new data, so we need to create a new buffer
        retData = std::make_unique<InProcBuffer>(outData, outDataSize);
    }
    else
    {
        //Otherwise, just resize to the new size to communicate to the caller
        retData->Resize(outDataSize);
    }

    if(result < 0)
    {
        return Exception(reinterpret_cast<const char*>(retData->Data()));
    }
    return retData;
}
}