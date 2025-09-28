#ifndef NODE_H
#define NODE_H

#include "IMessage.h"
#include <iosfwd>
#include <memory>
#include <string>

class Node {
public:
    using input_stream  = std::istream;
    using output_stream = std::ostream;

    Node(input_stream& input, output_stream& output) : in_(input), out_(output) {}

    void sendMessage(IMessage const& message);

    std::unique_ptr<IMessage> receiveMessage();

private:
    input_stream&  in_;
    output_stream& out_;
};

#endif // NODE_H