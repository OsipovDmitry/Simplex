#ifndef SINGLETOON_H
#define SINGLETOON_H

#define SINGLETON(Class, params...) \
    public: \
    static Class& instance() { \
        static Class p{params}; \
        return p; \
    }


#endif
