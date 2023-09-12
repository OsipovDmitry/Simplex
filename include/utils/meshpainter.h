#ifndef UTILS_MESHPAINTER_H
#define UTILS_MESHPAINTER_H

#include <memory>
#include <vector>
#include <unordered_map>

#include <utils/utilsglobal.h>
#include <utils/forwarddecl.h>
#include <utils/glm/vec4.hpp>
#include <utils/glm/mat4x4.hpp>

namespace simplex
{
namespace utils
{

class AbstractPainterPrivate;
class UTILS_SHARED_EXPORT AbstractPainter
{
public:
    AbstractPainter(std::shared_ptr<Mesh>);
    virtual ~AbstractPainter();

    std::shared_ptr<const Mesh> mesh() const;
    std::shared_ptr<Mesh> mesh();
    void setMesh(std::shared_ptr<Mesh>);

    const glm::vec4 &defaultValue(VertexAttribute) const;
    void setDefaultValue(VertexAttribute, const glm::vec4&);

    const glm::mat4 &vertexTransform() const;
    const glm::mat4 &normalTransform() const;
    void setVertexTransform(const glm::mat4&);

    const glm::mat4 &texCoordTransform() const;
    void setTexCoordTransform(const glm::mat4&);

    void drawArrays(const std::unordered_map<VertexAttribute, const std::vector<glm::vec4>&> &vertices,
                    PrimitiveType primitiveType);

    void drawElements(const std::unordered_map<VertexAttribute, const std::vector<glm::vec4>&> &vertices,
                      PrimitiveType primitiveType,
                      const std::vector<uint32_t> &indices,
                      DrawElementsIndexType drawElemetsBufferType);

protected:
    std::unique_ptr<AbstractPainterPrivate> m_;

    std::pair<uint32_t, uint32_t> addVertices(const std::unordered_map<VertexAttribute, const std::vector<glm::vec4>&> &vertices);
};

class UTILS_SHARED_EXPORT MeshPainter : public AbstractPainter
{
public:
    explicit MeshPainter(std::shared_ptr<Mesh>);
    ~MeshPainter() override;

    void drawTriangle();
    void drawPlane();
    void drawTetrahedron();
    void drawCube();
    void drawBoundingBox();
    void drawMonkey();
    void drawTeapot();
    void drawSphere(uint32_t = 8u);
    void drawCamera();
    void drawScreenQuad();
};

}
}

#endif // UTILS_MESHPAINTER_H
