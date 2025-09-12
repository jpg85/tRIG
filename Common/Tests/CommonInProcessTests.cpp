#include "Rig/Transport/Server/Function.h"
#include <rigtransport_inprocess.h>
#include <rigtransport_inprocess_server.h>

#include <array>
#include <catch2/catch_test_macros.hpp>

namespace Rig::Transport::Tests
{
std::optional<Exception> InvalidFunction(IBuffer& data)
{
    return Exception("Function not implemented");
}
std::optional<Exception> NoOpFunction(IBuffer& data)
{
    data.Resize(0);
    return std::nullopt;
}
static const std::array<std::pair<std::string_view, Server::FunctionT>, 2> RegisteredFunctions{
    std::pair{"Invalid", (Server::FunctionT)&InvalidFunction},
    std::pair{"Valid", (Server::FunctionT)&NoOpFunction}
};
static const std::array<const char*, 3> RegisteredFunctionNames{
    "Invalid",
    "Valid",
    "Unknown"
};

TEST_CASE("rigtransport_inprocess error checking", "[inproc]")
{
    SECTION("rigtransport_inprocess_findfunctions invalid parameters")
    {
        uint32_t apiIdx;
        auto result = rigtransport_inprocess_findfunctions(nullptr, 1, &apiIdx);
        REQUIRE(result != 0);
        REQUIRE(std::string(rigtransport_inprocess_geterrorstring(result)) == "Invalid parameter");
        const char* functionList[1] = { "NoOpFunction" };
        result = rigtransport_inprocess_findfunctions(functionList, 0, &apiIdx);
        REQUIRE(result != 0);
        REQUIRE(std::string(rigtransport_inprocess_geterrorstring(result)) == "Invalid parameter");
        result = rigtransport_inprocess_findfunctions(functionList, 1, nullptr);
        REQUIRE(result != 0);
        REQUIRE(std::string(rigtransport_inprocess_geterrorstring(result)) == "Invalid parameter");
    }
    SECTION("rigtransport_inprocess_releasefunctions invalid parameters")
    {
        auto result = rigtransport_inprocess_releasefunctions(100);
        REQUIRE(result != 0);
        REQUIRE(std::string(rigtransport_inprocess_geterrorstring(result)) == "Invalid API index");
    }
    SECTION("rigtransport_inprocess_callfunction invalid parameters")
    {
        InProcess::RegisterFunctions(RegisteredFunctions);
        uint32_t validApiIdx = 0;
        auto findResult = rigtransport_inprocess_findfunctions(RegisteredFunctionNames.data(), static_cast<uint32_t>(RegisteredFunctionNames.size()), &validApiIdx);
        REQUIRE(findResult == 0);
        uint8_t* outData = nullptr;
        uint32_t outDataSize = 0;
        auto result = rigtransport_inprocess_callfunction(100, 0, nullptr, 0, &outData, &outDataSize);
        REQUIRE(result != 0);
        REQUIRE(std::string(rigtransport_inprocess_geterrorstring(result)) == "Invalid API index");
        result = rigtransport_inprocess_callfunction(validApiIdx, 100, nullptr, 0, &outData, &outDataSize);
        REQUIRE(result != 0);
        REQUIRE(std::string(rigtransport_inprocess_geterrorstring(result)) == "Invalid function index");
        result = rigtransport_inprocess_callfunction(validApiIdx, 0, nullptr, 0, nullptr, &outDataSize);
        REQUIRE(result != 0);
        REQUIRE(std::string(rigtransport_inprocess_geterrorstring(result)) == "Invalid out data pointer");
        result = rigtransport_inprocess_callfunction(validApiIdx, 0, nullptr, 0, &outData, nullptr);
        REQUIRE(result != 0);
        REQUIRE(std::string(rigtransport_inprocess_geterrorstring(result)) == "Invalid out data size pointer");
        result = rigtransport_inprocess_callfunction(validApiIdx, 0, nullptr, 1, &outData, &outDataSize);
        REQUIRE(result != 0);
        REQUIRE(std::string(rigtransport_inprocess_geterrorstring(result)) == "Invalid data pointer");
    }
    SECTION("rigtransport_inprocess_free with nullptr does not crash")
    {
        rigtransport_inprocess_free(nullptr);
    }
    SECTION("rigtransport_inprocess_geterrorstring with unknown error code")
    {
        auto errorString = rigtransport_inprocess_geterrorstring(100);
        REQUIRE(std::string(errorString) == "Unknown error code");
    }
}
TEST_CASE("rigtransport_inprocess call functions", "[inproc]")
{
    InProcess::RegisterFunctions(RegisteredFunctions);
    uint32_t apiIdx = 0;
    auto findResult = rigtransport_inprocess_findfunctions(RegisteredFunctionNames.data(), static_cast<uint32_t>(RegisteredFunctionNames.size()), &apiIdx);
    REQUIRE(findResult == 0);
    uint8_t* outData = nullptr;
    uint32_t outDataSize = 0;
    SECTION("Call valid No-Op function")
    {
        auto result = rigtransport_inprocess_callfunction(apiIdx, 1, nullptr, 0, &outData, &outDataSize);
        REQUIRE(result == 0);
        REQUIRE(outData == nullptr);
        REQUIRE(outDataSize == 0);
    }
    SECTION("Call invalid function")
    {
        auto result = rigtransport_inprocess_callfunction(apiIdx, 0, nullptr, 0, &outData, &outDataSize);
        REQUIRE(result < 0);
        REQUIRE(std::string(rigtransport_inprocess_geterrorstring(result)) == "Function call failed, see output data for details");
        REQUIRE(outData != nullptr);
        REQUIRE(outDataSize > 0);
        REQUIRE(std::string(reinterpret_cast<char*>(outData)) == "Function not implemented");
        rigtransport_inprocess_free(outData);
    }
    SECTION("Call missing function")
    {
        auto result = rigtransport_inprocess_callfunction(apiIdx, 2, nullptr, 0, &outData, &outDataSize);
        REQUIRE(result < 0);
        REQUIRE(std::string(rigtransport_inprocess_geterrorstring(result)) == "Function call failed, see output data for details");
        REQUIRE(outData != nullptr);
        REQUIRE(outDataSize > 0);
        REQUIRE(std::string(reinterpret_cast<char*>(outData)) == "Function not found");
        rigtransport_inprocess_free(outData);
    }
}
}