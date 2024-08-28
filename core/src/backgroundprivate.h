#ifndef CORE_BACKGROUNDPRIVATE_H
#define CORE_BACKGROUNDPRIVATE_H

#include <memory>

namespace simplex
{
namespace core
{

class BackgroundDrawable;

class BackgroundPrivate
{
public:
    BackgroundPrivate();
    ~BackgroundPrivate();

    std::shared_ptr<BackgroundDrawable> &drawable();

private:
    std::shared_ptr<BackgroundDrawable> m_drawable;

};

}
}

#endif // CORE_BACKGROUNDPRIVATE_H
