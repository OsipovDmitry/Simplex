#ifndef CORE_RENDERINFO_H
#define CORE_RENDERINFO_H

#include <core/coreglobal.h>
#include <core/forwarddecl.h>
#include <core/stateset.h>

namespace simplex
{
namespace core
{

class CORE_SHARED_EXPORT RenderInfo : public StateSet
{
public:
    RenderInfo();
    ~RenderInfo() override;

    void setGBuffer(const graphics::PConstTexture& colorMap,
                    const graphics::PConstTexture& depthMap,
                    const graphics::PConstImage& depthImage,
                    const graphics::PConstImage& indicesImage,
                    const graphics::PConstBuffer& nodesBuffer,
                    const graphics::PConstBuffer& nodesCounter);

    void setSSAOMap(const graphics::PConstTexture&);

    void setLayeredShadowMatricesBuffer(const graphics::PConstBuffer&);

};

}
}

#endif // CORE_RENDERINFO_H
