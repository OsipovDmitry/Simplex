#ifndef CORE_IBLLIGHTNODEPRIVATE_H
#define CORE_IBLLIGHTNODEPRIVATE_H

#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

class IBLLightNodePrivate : public LightNodePrivate
{
public:
    IBLLightNodePrivate(const std::string &name);
    ~IBLLightNodePrivate() override;

    static std::weak_ptr<graphics::IVertexArray> &lightAreaVertexArray();
    static std::weak_ptr<const graphics::ITexture> &defaultDiffuseTexture();
    static std::weak_ptr<const graphics::ITexture> &defaultSpecularTexture();

private:
    static std::weak_ptr<graphics::IVertexArray> s_lightAreaVertexArray;
    static std::weak_ptr<const graphics::ITexture> s_defaultDiffuseTexture;
    static std::weak_ptr<const graphics::ITexture> s_defaultSpecularTexture;

};

}
}

#endif // CORE_IBLLIGHTNODEPRIVATE_H
