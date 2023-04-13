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

    std::shared_ptr<Mesh> mesh() const;

    const glm::vec4 &defaultValue(VertexAttribute) const;
    void setDefaultValue(VertexAttribute, const glm::vec4&);

    const glm::mat4x4 &vertexTransform() const;
    const glm::mat4x4 &normalTransform() const;
    void setVertexTransform(const glm::mat4x4&);

    const glm::mat4x4 &texCoordTransform() const;
    void setTexCoordTransform(const glm::mat4x4&);

protected:
    std::unique_ptr<AbstractPainterPrivate> m_;
};

class UTILS_SHARED_EXPORT MeshPainter : public AbstractPainter
{
public:
    MeshPainter(std::shared_ptr<Mesh>);

    void drawMesh(const std::unordered_map<VertexAttribute, const std::vector<glm::vec4>&> &vertices,
                  const std::vector<uint32_t> &indices);

    void drawTriangle();
    void drawCube();
    void drawMonkey();
    void drawTeapot();

};

}
}

#endif // UTILS_MESHPAINTER_H
