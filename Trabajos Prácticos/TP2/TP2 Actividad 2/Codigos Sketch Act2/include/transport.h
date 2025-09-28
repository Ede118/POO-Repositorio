#ifndef TRANSPORT_H
#define TRANSPORT_H

#include "serializer.h"

class ITransport {
public:
    virtual ~ITransport() = default;
    virtual void connect(const std::string& host, uint16_t port) = 0;
    virtual void send(const Message& m) = 0;
    virtual Message recv() = 0;
    virtual void close() = 0;
};

#endif