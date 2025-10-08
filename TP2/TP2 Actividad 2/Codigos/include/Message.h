#ifndef MESSAGE_H
#define MESSAGE_H

#include "AbstractMessage.h"
#include <string>

class Message : public AbstractMessage {
public:
    Message(int op, DataBag data) : AbstractMessage(op, std::move(data)) {}
    std::string toJSON() const override;
};

#endif // MESSAGE_H