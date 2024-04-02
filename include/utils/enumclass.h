#ifndef UTILS_ENUMCLASS_H
#define UTILS_ENUMCLASS_H

#include <cstddef>
#include <type_traits>

namespace simplex
{
namespace utils
{

template <typename T>
[[maybe_unused]] static constexpr T castToEnumClass(typename std::underlying_type<T>::type value) { return static_cast<T>(value); }

template <typename T>
[[maybe_unused]] static constexpr typename std::underlying_type<T>::type castFromEnumClass(T value) { return static_cast<typename std::underlying_type<T>::type>(value); }

template <typename T>
[[maybe_unused]] static constexpr size_t numElementsEnumClass() { return static_cast<size_t>(T::Count); }

} // namespace
} // namespace

#define ENUMCLASS(Name, type, ...) \
    enum class Name : type { \
        __VA_ARGS__ , Count \
    }; \
    [[maybe_unused]] static constexpr Name castTo##Name(type value) { return simplex::utils::castToEnumClass<Name>(value); } \
    [[maybe_unused]] static constexpr type castFrom##Name(Name value) { return simplex::utils::castFromEnumClass<Name>(value); } \
    [[maybe_unused]] static constexpr size_t numElements##Name() { return simplex::utils::numElementsEnumClass<Name>(); }

#endif // UTILS_ENUMCLASS_H
