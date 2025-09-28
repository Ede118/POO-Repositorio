#include "serializer.h"

#include "Envelope.h"

#include <chrono>
#include <optional>
#include <stdexcept>
#include <string>

namespace {
constexpr const char* kMeta = "meta";
constexpr const char* kPayload = "payload";
constexpr const char* kOpCode = "op_code";
constexpr const char* kUserId = "user_id";
constexpr const char* kDeviceId = "device_id";
constexpr const char* kTimestamp = "timestamp_ms";
constexpr const char* kCorrelationId = "correlation_id";

std::string bytesToString(const Bytes& bytes) {
    std::string out;
    out.reserve(bytes.size());
    for (std::byte b : bytes) {
        out.push_back(static_cast<char>(b));
    }
    return out;
}

Bytes stringToBytes(const std::string& s) {
    Bytes bytes;
    bytes.reserve(s.size());
    for (unsigned char c : s) {
        bytes.push_back(static_cast<std::byte>(c));
    }
    return bytes;
}
} // namespace

Bytes JsonSerializar::toBytes(const Message& m) {
    Json metaJson;
    metaJson[kOpCode] = static_cast<int>(m.meta().opCode());
    if (const auto& user = m.meta().userId()) {
        metaJson[kUserId] = *user;
    }
    if (const auto& device = m.meta().deviceId()) {
        metaJson[kDeviceId] = *device;
    }
    const auto ts = std::chrono::duration_cast<std::chrono::milliseconds>(
        m.meta().timestamp().time_since_epoch());
    metaJson[kTimestamp] = static_cast<std::int64_t>(ts.count());
    metaJson[kCorrelationId] = m.meta().correlationId();

    Json root;
    root[kMeta] = std::move(metaJson);
    root[kPayload] = m.payload();

    return stringToBytes(root.dump());
}

Message JsonSerializar::fromBytes(const Bytes& b) {
    Json root = Json::parse(bytesToString(b));
    if (!root.contains(kMeta)) {
        throw std::runtime_error("Serialized message missing 'meta'");
    }
    const Json& metaJson = root.at(kMeta);
    if (!metaJson.contains(kOpCode)) {
        throw std::runtime_error("Serialized message missing 'op_code'");
    }

    auto opInt = metaJson.at(kOpCode).get<int>();
    if (opInt < static_cast<int>(OpCode::Unknown) || opInt > static_cast<int>(OpCode::Error)) {
        throw std::runtime_error("Serialized message with invalid op_code value");
    }
    std::optional<std::string> user;
    if (metaJson.contains(kUserId) && !metaJson.at(kUserId).is_null()) {
        user = metaJson.at(kUserId).get<std::string>();
    }
    std::optional<std::string> device;
    if (metaJson.contains(kDeviceId) && !metaJson.at(kDeviceId).is_null()) {
        device = metaJson.at(kDeviceId).get<std::string>();
    }

    std::int64_t millis = 0;
    if (metaJson.contains(kTimestamp)) {
        millis = metaJson.at(kTimestamp).get<std::int64_t>();
    }
    Envelope::Clock::time_point ts{std::chrono::milliseconds{millis}};
    std::string corrId = metaJson.contains(kCorrelationId)
                             ? metaJson.at(kCorrelationId).get<std::string>()
                             : std::string{};

    Envelope env(static_cast<OpCode>(opInt), std::move(user), std::move(device), std::move(corrId), ts);
    Json payload = root.contains(kPayload) ? root.at(kPayload) : Json::object();
    return Message(std::move(env), std::move(payload));
}
