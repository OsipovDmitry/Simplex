#ifndef CORE_SPOTLIGHTNODEPRIVATE_H
#define CORE_SPOTLIGHTNODEPRIVATE_H

#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

class SpotLightNodePrivate : public LightNodePrivate
{
public:
    SpotLightNodePrivate(const std::string &name);
    ~SpotLightNodePrivate() override;

    static std::weak_ptr<graphics::IVertexArray> &lightAreaVertexArray();

private:
    static std::weak_ptr<graphics::IVertexArray> s_lightAreaVertexArray;

};

}
}

#endif // CORE_SPOTLIGHTNODEPRIVATE_H
