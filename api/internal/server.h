#ifndef APPSERVER_H
#define APPSERVER_H

#include "api/internal/def.h"

namespace app{
namespace server{

I_API(ServerStatus){
    API_FIELD(Status) // M, see src/enums/status.h
}

} // server
} // app

#endif // APPSERVER_H
