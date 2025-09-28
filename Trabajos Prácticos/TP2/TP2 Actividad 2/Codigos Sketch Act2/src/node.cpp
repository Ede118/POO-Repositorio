#include "node.h"

#include <stdexcept>

namespace {
constexpr const char* kSendPath = "logs/outbox_message.json";
constexpr const char* kRecvPath = "logs/inbox_message.json";
}

void Node::sendMessage(Message m) {
    if (!m.meta().isValid(false)) {
        throw std::runtime_error("Attempted to send message with invalid metadata");
    }
    repo_.save(m, kSendPath);
    net_.send(m);
}

Message Node::receiveOnce(bool requireIds) {
    Message msg = net_.recv();
    if (!msg.meta().isValid(requireIds)) {
        throw std::runtime_error("Received message with invalid metadata");
    }
    repo_.save(msg, kRecvPath);
    return msg;
}
