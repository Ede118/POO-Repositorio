// main.cpp
#include "Node.h"
#include "Message.h"
#include "AuthMessage.h"
#include <sstream>
#include <iostream>

int main() {
    std::istringstream in(
        "{\"type\":\"message\",\"op\":42}\n"
        "{\"type\":\"auth\",\"op\":7,\"userID\":\"u\",\"deviceID\":\"d\"}\n"
    );
    std::ostringstream out;
    Node node(in, out);

    // en tu main de prueba, antes de send/receive:
    DataBag db;
    db.addColumn<DataBag::ColInt>("id",    {1,2,3});
    db.addColumn<DataBag::ColReal>("temp", {21.5, 22.0, 23.1});
    db.addColumn<DataBag::ColBool>("ok",   {true, false, true});
    db.addColumn<DataBag::ColText>("name", {"a","b","c"});

    Message m(99, db);
    node.sendMessage(m); // el out ahora llevará el bloque "data"


    // receive 1
    auto msg1 = node.receiveMessage();
    std::cout << msg1->toJSON() << "\n";

    // receive 2
    auto msg2 = node.receiveMessage();
    std::cout << msg2->toJSON() << "\n";

    std::cout << "OUT BUF: " << out.str() << "\n";
}
