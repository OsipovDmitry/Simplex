#ifndef UTILS_MESHPAINTER_H
#define UTILS_MESHPAINTER_H

#include <memory>
#include <vector>
#include <unordered_map>

#include <utils/glm/vec3.hpp>
#include <utils/glm/vec4.hpp>
#include <utils/utilsglobal.h>
#include <utils/forwarddecl.h>
#include <utils/boundingbox.h>

namespace simplex
{
namespace utils
{

class AbstractPainterPrivate;
class UTILS_SHARED_EXPORT AbstractPainter
{
public:
    AbstractPainter(const std::shared_ptr<Mesh>&);
    virtual ~AbstractPainter();

    std::shared_ptr<const Mesh> mesh() const;
    std::shared_ptr<Mesh> mesh();
    void setMesh(std::shared_ptr<Mesh>);

    const glm::vec4 &defaultValue(VertexAttribute) const;
    void setDefaultValue(VertexAttribute, const glm::vec4&);

    const Transform &vertexTransform() const;
    const Transform &normalTransform() const;
    void setVertexTransform(const Transform&);

    const Transform &texCoordsTransform() const;
    void setTexCoordsTransform(const Transform&);

    void drawArrays(const std::unordered_map<VertexAttribute, const std::vector<glm::vec4>&> &vertices,
                    PrimitiveType primitiveType);

    void drawElements(const std::unordered_map<VertexAttribute, const std::vector<glm::vec4>&> &vertices,
                      PrimitiveType primitiveType,
                      const std::vector<uint32_t> &indices,
                      DrawElementsIndexType drawElemetsBufferType);

    BoundingBox calculateBoundingBox() const;

protected:
    std::unique_ptr<AbstractPainterPrivate> m_;
};

class UTILS_SHARED_EXPORT MeshPainter : public AbstractPainter
{
public:
    explicit MeshPainter(const std::shared_ptr<Mesh>&);
    ~MeshPainter() override;

    MeshPainter &drawTriangle();
    MeshPainter &drawPlane();
    MeshPainter &drawTetrahedron();
    MeshPainter &drawCube(const glm::vec3& = glm::vec3(1.f));
    MeshPainter &drawSuzanne();
    MeshPainter &drawBunny();
    MeshPainter &drawTeapot();
    MeshPainter &drawSphere(uint32_t = 8u);
    MeshPainter &drawCone(uint32_t = 8u);
    MeshPainter &drawCamera();
    MeshPainter &drawBoundingBox();
    MeshPainter &drawScreenQuad();
};

}
}

#endif // UTILS_MESHPAINTER_H
