#ifndef NODE_H
#define NODE_H

#include "repository.h"
#include "transport.h"

class Node {
public:
    Node(ITransport& t, IRepository& r): net_(t), repo_(r) {}
    // API que tu consigna necesita
    void sendMessage(Message m);     // TODO: validar, persistir, enviar
    Message receiveOnce(bool requireIds); // TODO: recibir, validar, persistir
private:
    ITransport& net_;
    IRepository& repo_;
};

#endif