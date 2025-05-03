#ifndef RADIO_H
#define RADIO_H

#include "api/external/def.h"

namespace api{
namespace radio{

API(RadioStationListRequest){
}
API(RadioStationListResponse){
    API_FIELD(RadioStationList) // M, QVariantList, List of RadioStation, see API(RadioStation)
}

API(RadioStation){
    API_FIELD(Id)    // M, uint64_t
    API_FIELD(Name)  // M, QString
    API_FIELD(URL)   // M, QString
    API_FIELD(Image) // O, QByteArray, .jpg
}

} // server
} // api

#endif // RADIO_H
