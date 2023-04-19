#ifndef UTILS_FORWARDDECL_H
#define UTILS_FORWARDDECL_H

#include <cstdint>

namespace simplex
{
namespace utils
{

enum class Type : uint16_t;
enum class PrimitiveType : uint16_t;
enum class VertexAttribute : uint16_t;

class Buffer;
class VertexBuffer;
class DrawElementsBuffer;
class Mesh;

class Transform;
class Plane;
class BoundingBox;

class AbstractClipSpace;
class OrthoClipSpace;
class PerspectiveClipSpace;

class PrimitiveSet;
class DrawArrays;
class DrawElements;

}
}

#endif // UTILS_FORWARDDECL_H
