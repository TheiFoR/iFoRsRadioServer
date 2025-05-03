#ifndef APISERVER_H
#define APISERVER_H

#include "api/external/def.h"

namespace api{
namespace server{

API(ServerConnectionRequest){
}

API(ServerConnectionResponse){
    API_FIELD(Confirmation)
}

} // server
} // api

#endif //  APISERVER_H
