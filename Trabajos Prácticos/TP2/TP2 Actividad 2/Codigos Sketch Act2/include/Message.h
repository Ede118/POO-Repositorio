#pragma once

#include "Envelope.h"
#include "json.hpp"

#include <stdexcept>
#include <string>
#include <utility>

using Json = nlohmann::json;

class Message {
public:
    Message(Envelope meta, Json payload = Json::object());

    const Envelope& meta() const noexcept;
    Envelope& meta() noexcept;
    const Json& payload() const noexcept;
    void setPayload(Json p);

    template <typename T>
    void put(const std::string& key, T&& value);

    bool has(const std::string& key) const;

    template <typename T>
    T get(const std::string& key) const;

private:
    Envelope meta_;
    Json payload_;
};

template <typename T>
void Message::put(const std::string& key, T&& value) {
    payload_[key] = std::forward<T>(value);
}

template <typename T>
T Message::get(const std::string& key) const {
    if (!payload_.contains(key)) {
        throw std::out_of_range("Payload key not found: " + key);
    }
    return payload_.at(key).get<T>();
}
