#include "Message.h"

Message::Message(Envelope meta, Json payload)
    : meta_(std::move(meta)), payload_(std::move(payload)) {}

const Envelope& Message::meta() const noexcept { return meta_; }
Envelope& Message::meta() noexcept { return meta_; }
const Json& Message::payload() const noexcept { return payload_; }

void Message::setPayload(Json p) {
    payload_ = std::move(p);
}

bool Message::has(const std::string& key) const { return payload_.contains(key); }
