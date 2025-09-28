#include "Message.h"
#include "DataBag.h"
#include <sstream>
using std::string;

string Message::toJSON() const {
    std::ostringstream os;
    os << "{\"type\":\"message\",\"op\":" << opCode();
    if (!data().empty()) {
        os << ",\"data\":" << data().toJSON();
    }
    os << '}';
    return os.str();
}
