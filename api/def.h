#ifndef DEF_H
#define DEF_H

#define API_BASE(name) \
namespace name{\
    constexpr char __name__[] = #name;\
}\
namespace name

#define API_FIELD(name) \
    constexpr char name[] = #name;

#endif // DEF_H
