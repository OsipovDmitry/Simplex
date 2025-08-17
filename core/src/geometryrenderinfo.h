#ifndef CORE_GEOMETRYRENDERINFO_H
#define CORE_GEOMETRYRENDERINFO_H

#include <utils/glm/mat4x4.hpp>

#include <core/forwarddecl.h>
#include <core/stateset.h>

namespace simplex
{
namespace core
{

class GeometryBuffer;
class OITNodesBuffer;

class GeometryRenderInfo : public StateSet
{
public:
    GeometryRenderInfo();
    ~GeometryRenderInfo() override;

    const glm::mat4x4& viewMatrix() const;
    void setViewMatrix(const glm::mat4x4&);

    const glm::mat4x4& projectionMatrix() const;
    void setProjectionMatrix(const glm::mat4x4&);

    std::shared_ptr<const GeometryBuffer> geometryBuffer() const;
    void setGeometryBuffer(const std::shared_ptr<const GeometryBuffer>&);

    void setSSAOMap(const graphics::PConstTexture&);

private:
    void setViewProjectionMatrix(const glm::mat4x4&, const glm::mat4x4&);

    glm::mat4x4 m_viewMatrix;
    glm::mat4x4 m_projectionMatrix;
    std::shared_ptr<const GeometryBuffer> m_geometryBuffer;
};

}
}

#endif // CORE_GEOMETRYRENDERINFO_H
