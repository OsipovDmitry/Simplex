#ifndef UTILS_PIMPL_H
#define UTILS_PIMPL_H

#define PIMPL(Class) \
    public: \
    template <typename T = Class##Private> inline T &m() { return dynamic_cast<T&>(*m_); } \
    template <typename T = Class##Private> inline T &m() const { return dynamic_cast<T&>(*m_); } \

#endif // UTILS_PIMPL_H
