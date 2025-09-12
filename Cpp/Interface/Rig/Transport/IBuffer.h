#pragma once
#include <cstdint>
#include <span>

namespace Rig::Transport
{
struct IBuffer
{
    virtual ~IBuffer() = default;
    virtual std::uint8_t* Data() = 0;
    virtual std::uint32_t Size() const = 0;
    virtual std::uint32_t MaxSize() const = 0;
    virtual void Resize(std::uint32_t size) = 0;

    std::span<std::uint8_t> AsSpan() { return {Data(), Size()}; }
};
}