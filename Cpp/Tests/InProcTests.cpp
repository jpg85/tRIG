#include <Rig/Transport/Exception.h>
#include <Rig/Transport/Client/Transport.h>
#include <Rig/Transport/Server/Transport.h>

#include <catch2/catch_test_macros.hpp>
#include <string_view>

namespace Rig::Transport::Tests
{
std::optional<Exception> ExceptionFunction(IBuffer&)
{
    return Exception("This function always fails");
}
std::optional<Exception> NoOpFunction(IBuffer& data)
{
    data.Resize(0);
    return std::nullopt;
}
std::optional<Exception> SmallData(IBuffer& data)
{
    data.Resize(1);
    return std::nullopt;
}
std::optional<Exception> LargeData(IBuffer& data)
{
    data.Resize(1024 * 1024);
    return std::nullopt;
}
static const std::array<std::pair<std::string_view, Server::FunctionT>, 4> RegisteredFunctions{
    std::pair{"ExceptionFunction", (Server::FunctionT)&ExceptionFunction},
    std::pair{"NoOpFunction", (Server::FunctionT)&NoOpFunction},
    std::pair{"SmallData", (Server::FunctionT)&SmallData},
    std::pair{"LargeData", (Server::FunctionT)&LargeData},
};
static const std::array<std::string_view, 4> RegisteredFunctionNames{
    "ExceptionFunction",
    "NoOpFunction",
    "SmallData",
    "LargeData",
};
struct SimpleBuffer : IBuffer
{
public:
    SimpleBuffer(uint32_t size)
        : m_data(size)
    {
    }
    ~SimpleBuffer() override = default;
    std::uint8_t* Data() override { return m_data.data(); }
    std::uint32_t Size() const override { return static_cast<std::uint32_t>(m_data.size()); }
    std::uint32_t MaxSize() const override { return static_cast<std::uint32_t>(m_data.max_size()); }
    void Resize(std::uint32_t size) override
    {
        m_data.resize(size);
    }
private:
    std::vector<std::uint8_t> m_data;
};

TEST_CASE("inproc integration", "[inproc]")
{
    auto server = Server::GetTransportFactory()->CreateTransport("inprocess://", RegisteredFunctions);
    auto client = Client::GetTransportFactory()->CreateTransport("inprocess://", RegisteredFunctionNames);

    SECTION("Call ExceptionFunction")
    {
        auto buffer = std::make_unique<SimpleBuffer>(256);
        auto result = client->CallFunction(0, std::move(buffer));
        REQUIRE(std::holds_alternative<Exception>(result));
        auto& exception = std::get<Exception>(result);
        REQUIRE(exception.what() == std::string("This function always fails"));
    }
    SECTION("Call NoOpFunction")
    {
        auto buffer = std::make_unique<SimpleBuffer>(256);
        auto result = client->CallFunction(1, std::move(buffer));
        REQUIRE(std::holds_alternative<std::unique_ptr<IBuffer>>(result));
        auto& outBuffer = std::get<std::unique_ptr<IBuffer>>(result);
        REQUIRE(outBuffer);
        REQUIRE(outBuffer->Size() == 0);
    }
    SECTION("Call SmallData")
    {
        auto buffer = std::make_unique<SimpleBuffer>(256);
        auto result = client->CallFunction(2, std::move(buffer));
        REQUIRE(std::holds_alternative<std::unique_ptr<IBuffer>>(result));
        auto& outBuffer = std::get<std::unique_ptr<IBuffer>>(result);
        REQUIRE(outBuffer);
        REQUIRE(outBuffer->Size() == 1);
    }
    SECTION("Call LargeData")
    {
        auto buffer = std::make_unique<SimpleBuffer>(256);
        auto result = client->CallFunction(3, std::move(buffer));
        REQUIRE(std::holds_alternative<std::unique_ptr<IBuffer>>(result));
        auto& outBuffer = std::get<std::unique_ptr<IBuffer>>(result);
        REQUIRE(outBuffer);
        REQUIRE(outBuffer->Size() == 1024 * 1024);
    }
}
}