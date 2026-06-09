#ifndef UTILS_CUSTOMDATA_H
#define UTILS_CUSTOMDATA_H

#include <memory>
#include <utility>

#define CUSTOMDATA(CustomDataClass)                                                                                              \
private:                                                                                                                         \
    std::unique_ptr<CustomDataClass> cd_;                                                                                        \
                                                                                                                                 \
public:                                                                                                                          \
    template <typename T = CustomDataClass, typename... Types>                                                                   \
    void initializeCustomData(Types&&... args)                                                                                   \
    {                                                                                                                            \
        cd_ = std::make_unique<T>(std::forward<Types>(args)...);                                                                 \
    }                                                                                                                            \
    template <typename T = CustomDataClass>                                                                                      \
    bool hasCustomData() const                                                                                                   \
    {                                                                                                                            \
        static_assert(std::is_base_of_v<CustomDataClass, T>, "Template parameter T must inherit from CustomDataClass");          \
        return dynamic_cast<T*>(cd_.get()) != nullptr;                                                                           \
    }                                                                                                                            \
    template <typename T = CustomDataClass>                                                                                      \
    const T& customData() const                                                                                                  \
    {                                                                                                                            \
        static_assert(std::is_base_of_v<CustomDataClass, T>, "Template parameter T must inherit from CustomDataClass");          \
        return *dynamic_cast<const T*>(cd_.get());                                                                               \
    }                                                                                                                            \
    template <typename T = CustomDataClass>                                                                                      \
    T& customData()                                                                                                              \
    {                                                                                                                            \
        static_assert(std::is_base_of_v<CustomDataClass, T>, "Template parameter T must inherit from CustomDataClass");          \
        return *dynamic_cast<T*>(cd_.get());                                                                                     \
    }

#endif // UTILS_CUSTOMDATA_H
