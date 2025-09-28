#include "AuthMessage.h"
#include "DataBag.h"
#include <sstream>
using std::string;

string AuthMessage::toJSON() const {
    std::ostringstream os;
    os << "{\"type\":\"auth\",\"op\":" << opCode()
       << ",\"userID\":\""  << userID_  << "\""
       << ",\"deviceID\":\"" << deviceID_ << "\"";
    if (!data().empty()) {
        os << ",\"data\":" << data().toJSON();
    }
    os << '}';
    return os.str();
}
