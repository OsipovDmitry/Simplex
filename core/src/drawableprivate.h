#ifndef CORE_DRAWABLEPRIVATE_H
#define CORE_DRAWABLEPRIVATE_H

#include <memory>
#include <string>
#include <unordered_map>

#include <utils/boundingbox.h>

#include <core/forwarddecl.h>

#include "statesetprivate.h"

namespace simplex
{
namespace core
{

class DrawablePrivate : public StateSetPrivate
{
public:
    DrawablePrivate(const std::shared_ptr<graphics::IVertexArray>&);
    ~DrawablePrivate() override;

    std::shared_ptr<graphics::IVertexArray> &vertexArray();

    bool &isDoubleSided();

protected:
    std::shared_ptr<graphics::IVertexArray> m_vertexArray;
    bool m_isDoubleSided;

};

}
}

#endif // CORE_DRAWABLEPRIVATE_H
