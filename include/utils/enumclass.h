#ifndef ENUMCLASS_H
#define ENUMCLASS_H

#define ENUMCLASS(Name, type, elements...) \
    enum class Name : type { \
        elements, Count \
    }; \
    constexpr Name castTo##Name(type value) { return static_cast< Name >(value); } \
    constexpr type castFrom##Name(Name value) { return static_cast< type >(value); }


#endif // ENUMCLASS_H
