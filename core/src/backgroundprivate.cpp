#include "backgroundprivate.h"

namespace simplex
{
namespace core
{

BackgroundPrivate::BackgroundPrivate()
{
}

BackgroundPrivate::~BackgroundPrivate() = default;

std::shared_ptr<BackgroundDrawable> &BackgroundPrivate::drawable()
{
    return m_drawable;
}

}
}
