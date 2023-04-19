#ifndef CORE_CAMERANODEPRIVATE_H
#define CORE_CAMERANODEPRIVATE_H

#include <utils/clipspace.h>

#include <core/forwarddecl.h>

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

class CameraNodePrivate : public NodePrivate
{
public:
    CameraNodePrivate(const std::string &name)
        : NodePrivate(name)
    {}

    std::shared_ptr<utils::AbstractClipSpace> &clipSpace() { return m_clipSpace; }
    bool &renderingEnabled() { return m_renderingEnabled; }

private:
    std::shared_ptr<utils::AbstractClipSpace> m_clipSpace;
    bool m_renderingEnabled;

};

}
}

#endif // CORE_CAMERANODEPRIVATE_H
