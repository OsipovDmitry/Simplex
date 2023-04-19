#ifndef UTILS_CONNECTABLE_H
#define UTILS_CONNECTABLE_H

#include <memory>

#include <unordered_set>

namespace simplex
{
namespace utils
{

class Connectable;
struct Equal
{
    bool operator()(const std::weak_ptr<simplex::utils::Connectable> &c1, const std::weak_ptr<simplex::utils::Connectable> &c2) const
    {
        if (c1.expired() || c2.expired()) return false;
        return c1.lock() == c2.lock();
    }
};

struct Hash
{
    size_t operator()(const std::weak_ptr<simplex::utils::Connectable> &c) const
    {
        return c.expired() ? 0u : std::hash<std::shared_ptr<simplex::utils::Connectable>>()(c.lock());
    }
};

class Connectable
{
public:
    virtual ~Connectable() = default;

    virtual void update(Connectable&) {}

    void connect(std::shared_ptr<Connectable> observer) { m_observers.insert(observer); }
    void disconnect(std::shared_ptr<Connectable> observer) { m_observers.erase(observer); }

protected:
    void notify()
    {
        for (auto it = m_observers.begin(); it != m_observers.end();)
        {
            if (it->expired())
                it = m_observers.erase(it);
            else
            {
                it->lock()->update(*this);
                ++it;
            }
        }
    }

private:
    std::unordered_set<std::weak_ptr<Connectable>, Hash, Equal> m_observers;
};

} // namespace
} // namespace

#endif // UTILS_CONNECTABLE_H
