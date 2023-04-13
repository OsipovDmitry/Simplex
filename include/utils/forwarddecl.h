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
class IndexBuffer;
class Mesh;

class Transform;
class Plane;
class BoundingBox;

}
}

#endif // UTILS_FORWARDDECL_H
