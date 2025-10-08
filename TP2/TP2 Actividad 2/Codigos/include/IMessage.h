#ifndef IMESSAGE_H
#define IMESSAGE_H

#include <memory>
#include <string>
#include <string_view>

class DataBag;  // forward decl limpia

struct IMessage {
    virtual ~IMessage() = default;

    virtual int opCode() const = 0;
    virtual DataBag const& data() const = 0;
    virtual std::string toJSON() const = 0;
};

[[nodiscard]] std::unique_ptr<IMessage> fromJSON(std::string_view jsonStr);

#endif // IMESSAGE_H