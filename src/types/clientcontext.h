#ifndef CLIENTCONTEXT_H
#define CLIENTCONTEXT_H

#include "src/network/clienthandler.h"
#include <QThread>

struct ClientContext{
    std::shared_ptr<ClientHandler> handler;
    std::shared_ptr<QThread> thread;
};


#endif // CLIENTCONTEXT_H
