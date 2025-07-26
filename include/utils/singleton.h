#ifndef UTILS_SINGLETON_H
#define UTILS_SINGLETON_H

#include <memory>

#define SINGLETON(Class) \
    public: \
    static Class &instance() { \
        static Class p; \
        return p; \
    }

#define SINGLETON_POINTER(Class) \
    public: \
    static std::shared_ptr<Class> instance() { \
        static std::shared_ptr<Class> p = std::shared_ptr<Class>(new Class); \
        return p; \
    }

#define SINGLETON_CONST_POINTER(Class) \
    public: \
    static std::shared_ptr<const Class> instance() { \
        static std::shared_ptr<Class> p = std::shared_ptr<Class>(new Class); \
        return p; \
    }

#endif // UTILS_SINGLETON_H
