#ifndef UTILS_SINGLETON_H
#define UTILS_SINGLETON_H

#define SINGLETON(Class) \
    public: \
    static Class &instance() { \
        static Class p; \
        return p; \
    }

#endif // UTILS_SINGLETON_H
