#ifndef UTILS_PIMPL_H
#define UTILS_PIMPL_H

#define PRIVATE_IMPL(Class) \
    public: \
    template <typename T = Class##Private> inline T &m() { return dynamic_cast<T&>(*m_); } \
    template <typename T = Class##Private> inline T &m() const { return dynamic_cast<T&>(*m_); }

#define PUBLIC_IMPL(Class) \
    public: \
    template <typename T = Class> inline T &d() { return dynamic_cast<T&>(d_); } \
    template <typename T = Class> inline T &d() const { return dynamic_cast<T&>(d_); }

#endif // UTILS_PIMPL_H
