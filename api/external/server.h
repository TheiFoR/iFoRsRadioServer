#ifndef APISERVER_H
#define APISERVER_H

#include "api/external/def.h"

namespace api{
namespace server{

API(ConnectionRequest){
}

API(ConnectionResponse){
    API_FIELD(Confirmation)
}

} // server
} // api

#endif //  APISERVER_H
