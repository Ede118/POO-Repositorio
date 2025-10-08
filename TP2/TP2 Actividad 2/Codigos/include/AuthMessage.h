#ifndef AUTHMESSAGE_H
#define AUTHMESSAGE_H

#include "AbstractMessage.h"
#include <string>
#include <utility>

class AuthMessage : public AbstractMessage {
public:
    AuthMessage(int op, DataBag data, std::string userID, std::string deviceID)
        : AbstractMessage(op, std::move(data)),
          userID_(std::move(userID)),
          deviceID_(std::move(deviceID)) {}

    std::string toJSON() const override;

    std::string const& getUserID() const { return userID_; }
    std::string const& getDevice() const { return deviceID_; }

private:
    std::string userID_;
    std::string deviceID_;
};

#endif // AUTHMESSAGE_H