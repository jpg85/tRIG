#pragma once
#include <stdexcept>

namespace Rig::Transport
{
class Exception : public std::runtime_error
{
public:
    explicit Exception(const std::string& message)
        : std::runtime_error(message) {}
};
}