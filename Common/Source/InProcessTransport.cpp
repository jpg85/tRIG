#include "rigtransport_inprocess.h"
#include "rigtransport_inprocess_server.h"

#include <cstdint>
#include <cstring>
#include <map>
#include <shared_mutex>
#include <vector>

using namespace Rig::Transport;
namespace 
{
struct ApiSet
{
    std::vector<Server::FunctionT> functions;
};
static std::vector<ApiSet> s_apiSets;
//Generally keeping a string_view as a map key is unsafe, but in this case the strings are static and exist for the lifetime of the program
static std::map<std::string_view, Server::FunctionT> s_registeredFunctions;
static std::shared_mutex s_registeredFunctionsMutex;

enum ErrorCode
{
    CallFailure = -1,
    Success = 0,
    InvalidParameter = 1,
    InvalidApiIndex = 2,
    InvalidFunctionIndex = 3,
    InvalidOutDataPointer = 4,
    InvalidOutDataSizePointer = 5,
    InvalidDataPointer = 6
};
class Buffer : public IBuffer
{
public:
    Buffer(uint8_t* data, uint32_t size)
        : m_data(data), m_originalSize(size), m_size(size)
    {
    }

    uint8_t* Data() override { return m_data; }
    uint32_t Size() const override { return m_size; }
    uint32_t MaxSize() const override { return m_originalSize; }
    void Resize(uint32_t size) override
    {
        if(size > m_originalSize)
        {
            m_data = new uint8_t[size];
            m_originalSize = size;
        }
        m_size = size;
    }
private:
    uint8_t* m_data;
    uint32_t m_originalSize;
    uint32_t m_size;
};
}

int32_t rigtransport_inprocess_findfunctions(const char* const* functionList, uint32_t functionCount, uint32_t* outApiIdx)
{
    if(functionList == nullptr)
        return InvalidParameter;
    if(functionCount == 0)
        return InvalidParameter;
    if(outApiIdx == nullptr)
        return InvalidParameter;

    std::lock_guard lock(s_registeredFunctionsMutex);
    ApiSet apiSet;
    apiSet.functions.resize(functionCount, Server::MissingFunction);
    for(uint32_t i = 0; i < functionCount; ++i)
    {
        auto it = s_registeredFunctions.find(functionList[i]);
        if(it != s_registeredFunctions.end())
            apiSet.functions[i] = it->second;
    }
    //First look to see if any index is free and then push back if it isn't
    for (uint32_t i = 0; i < s_apiSets.size(); ++i)
    {
        if (s_apiSets[i].functions.empty())
        {
            s_apiSets[i] = std::move(apiSet);
            *outApiIdx = i;
            return Success;
        }
    }
    s_apiSets.push_back(std::move(apiSet));
    *outApiIdx = static_cast<uint32_t>(s_apiSets.size() - 1);
    return Success;
}

int32_t rigtransport_inprocess_releasefunctions(uint32_t apiIdx)
{
    std::lock_guard lock(s_registeredFunctionsMutex);
    if(apiIdx >= s_apiSets.size())
        return InvalidApiIndex;
    s_apiSets[apiIdx].functions.clear();
    return Success;
}

int32_t rigtransport_inprocess_callfunction(uint32_t apiIdx, uint32_t functionId, uint8_t* data, uint32_t dataSize, uint8_t** outData, uint32_t* outDataSize)
{
    std::shared_lock lock(s_registeredFunctionsMutex);
    if(apiIdx >= s_apiSets.size())
        return InvalidApiIndex;
    const auto& apiSet = s_apiSets[apiIdx];
    if(functionId >= apiSet.functions.size())
        return InvalidFunctionIndex;
    if(data == nullptr && dataSize > 0)
        return InvalidDataPointer;
    if(outData == nullptr)
        return InvalidOutDataPointer;
    if(outDataSize == nullptr)
        return InvalidOutDataSizePointer;

    Buffer buffer(data, dataSize);
    //Call the actual function and check for an exception
    auto exception = apiSet.functions[functionId](buffer);
    if(exception.has_value())
    {
        buffer.Resize(static_cast<uint32_t>(strlen(exception->what()) + 1));
        auto data = buffer.AsSpan();
        std::memcpy(data.data(), exception->what(), data.size());
        *outData = buffer.Data();
        *outDataSize = buffer.Size();
        return CallFailure;
    }

    *outData = nullptr;
    *outDataSize = 0;
    if(buffer.Size() > 0)
    {
        *outDataSize = buffer.Size();
        *outData = buffer.Data();
    }
    return Success;
}

void rigtransport_inprocess_free(uint8_t* data)
{
    delete[] data;
}

const char* rigtransport_inprocess_geterrorstring(int32_t errorCode)
{
    switch(errorCode)
    {
    case CallFailure:
        return "Function call failed, see output data for details";
    case Success:
        return "Success";
    case InvalidParameter:
        return "Invalid parameter";
    case InvalidApiIndex:
        return "Invalid API index";
    case InvalidFunctionIndex:
        return "Invalid function index";
    case InvalidOutDataPointer:
        return "Invalid out data pointer";
    case InvalidOutDataSizePointer:
        return "Invalid out data size pointer";
    case InvalidDataPointer:
        return "Invalid data pointer";
    default:
        return "Unknown error code";
    }
}

void Rig::Transport::InProcess::RegisterFunctions(std::span<const std::pair<std::string_view, Server::FunctionT>> functions)
{
    std::lock_guard lock(s_registeredFunctionsMutex);
    for(const auto& func : functions)
    {
        s_registeredFunctions[func.first] = func.second;
    }
}
