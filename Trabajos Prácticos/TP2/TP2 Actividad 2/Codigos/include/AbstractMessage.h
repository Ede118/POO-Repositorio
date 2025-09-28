#ifndef ABSTRACTMESSAGE_H
#define ABSTRACTMESSAGE_H

#include "IMessage.h"
#include "DataBag.h"
#include <utility>

class AbstractMessage : public IMessage {
public:
    int opCode() const override { return op_; }
    DataBag const& data() const override { return data_; }
    virtual ~AbstractMessage() = default;

protected:
    explicit AbstractMessage(int op, DataBag data)
        : op_(op), data_(std::move(data)) {}

    int op_;
    DataBag data_;
};

#endif // ABSTRACTMESSAGE_H