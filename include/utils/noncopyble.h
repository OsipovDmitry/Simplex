#ifndef UTILS_NONCOPYBLE_H
#define UTILS_NONCOPYBLE_H

#define NONCOPYBLE(Class) \
    Class(const Class&) = delete; \
    Class &operator =(const Class&) = delete;

#endif // UTILS_NONCOPYBLE_H
