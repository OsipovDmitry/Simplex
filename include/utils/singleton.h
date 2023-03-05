#ifndef SINGLETON_H
#define SINGLETON_H

#define SINGLETON(Class) \
    public: \
    static Class& instance() { \
        static Class p; \
        return p; \
    }

#endif // SINGLETON_H
