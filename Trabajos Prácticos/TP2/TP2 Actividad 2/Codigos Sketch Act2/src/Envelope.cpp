#include "Envelope.h"

#include <utility>

Envelope::Envelope(OpCode op,
                   std::optional<std::string> userId,
                   std::optional<std::string> deviceId,
                   std::string correlationId,
                   Clock::time_point timestamp)
    : opCode_(op),
      userId_(std::move(userId)),
      deviceId_(std::move(deviceId)),
      timestamp_(timestamp),
      correlationId_(std::move(correlationId)) {}

OpCode Envelope::opCode() const noexcept { return opCode_; }
const std::optional<std::string>& Envelope::userId() const noexcept { return userId_; }
const std::optional<std::string>& Envelope::deviceId() const noexcept { return deviceId_; }
Envelope::Clock::time_point Envelope::timestamp() const noexcept { return timestamp_; }
const std::string& Envelope::correlationId() const noexcept { return correlationId_; }

bool Envelope::isValid(bool requireIds) const {
    if (requireIds) {
        const bool hasUser = userId_.has_value() && !userId_->empty();
        const bool hasDevice = deviceId_.has_value() && !deviceId_->empty();
        if (!hasUser || !hasDevice) {
            return false;
        }
    }
    return opCode_ != OpCode::Unknown;
}

void Envelope::setOpCode(OpCode op) noexcept { opCode_ = op; }

void Envelope::setUserAndDevice(std::string uid, std::string did) {
    userId_ = std::move(uid);
    deviceId_ = std::move(did);
}

void Envelope::setTimestamp(Clock::time_point ts) noexcept { timestamp_ = ts; }
