#ifndef UTILS_ENUMCLASS_H
#define UTILS_ENUMCLASS_H

#define ENUMCLASS(Name, type, ...) \
    enum class Name : type { \
        __VA_ARGS__ , Count \
    }; \
    using Name##InternalType = type; \
    [[maybe_unused]] static constexpr Name castTo##Name(type value) { return static_cast< Name >(value); } \
    [[maybe_unused]] static constexpr type castFrom##Name(Name value) { return static_cast< type >(value); } \
    [[maybe_unused]] static constexpr size_t numElements##Name() { return static_cast<size_t>(Name::Count); }

#endif // UTILS_ENUMCLASS_H
