#include "Node.h"
#include <ostream>
#include <istream>
#include <string>

using std::string;

void Node::sendMessage(IMessage const& message) {
    out_ << message.toJSON() << std::endl;
}

std::unique_ptr<IMessage> Node::receiveMessage() {
    string line;
    
    if (!std::getline(in_, line)) { return nullptr; }
    
    return fromJSON(line);
}