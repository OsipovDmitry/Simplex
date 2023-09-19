#ifndef CORE_POINTLIGHTNODEPRIVATE_H
#define CORE_POINTLIGHTNODEPRIVATE_H

#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

class PointLightNodePrivate : public LightNodePrivate
{
public:
    PointLightNodePrivate(const std::string &name);
    ~PointLightNodePrivate() override;

    static std::weak_ptr<graphics::IVertexArray> &lightAreaVertexArray();

private:
    static std::weak_ptr<graphics::IVertexArray> s_lightAreaVertexArray;

};

}
}

#endif // CORE_POINTLIGHTNODEPRIVATE_H
