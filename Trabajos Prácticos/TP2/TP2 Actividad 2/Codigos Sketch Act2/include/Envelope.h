#pragma once

#include <chrono>
#include <optional>
#include <string>

enum class OpCode : int { Unknown = 0, Ok = 1, Error = 2 };

class Envelope {
public:
    using Clock = std::chrono::system_clock;

    Envelope(OpCode op,
             std::optional<std::string> userId = std::nullopt,
             std::optional<std::string> deviceId = std::nullopt,
             std::string correlationId = "",
             Clock::time_point timestamp = Clock::now());

    OpCode opCode() const noexcept;
    const std::optional<std::string>& userId() const noexcept;
    const std::optional<std::string>& deviceId() const noexcept;
    Clock::time_point timestamp() const noexcept;
    const std::string& correlationId() const noexcept;

    bool isValid(bool requireIds) const;

    void setOpCode(OpCode op) noexcept;
    void setUserAndDevice(std::string uid, std::string did);
    void setTimestamp(Clock::time_point ts) noexcept;

private:
    OpCode opCode_;
    std::optional<std::string> userId_;
    std::optional<std::string> deviceId_;
    Clock::time_point timestamp_;
    std::string correlationId_;
};
