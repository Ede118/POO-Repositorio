#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "Message.h"
#include <vector>

using Bytes = std::vector<std::byte>;

class ISerializer {
    public:
        virtual ~ISerializer() = default;
        virtual Bytes toBytes(const Message& m) = 0;
        virtual Message fromBytes(const Bytes& b) = 0;
};

class JsonSerializar final: public ISerializer{
public:
    Bytes toBytes(const Message& m) override;
    Message fromBytes(const Bytes& b) override;
};

#endif // SERIALIZER