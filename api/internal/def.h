#ifndef INTERNALDEF_H
#define INTERNALDEF_H

#include "api/def.h"

#define I_API(name) \
namespace name{\
    constexpr char __name__[] = "I_"#name;\
}\
    namespace name

#endif // INTERNALDEF_H
