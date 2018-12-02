#ifndef SINGLETOON_H
#define SINGLETOON_H

#define SINGLETON(Class, params...) \
    public: \
    static Class& instance() { \
        static Class p = Class(params); \
        return p; \
    }


#endif
