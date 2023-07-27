#ifndef UTILS_FORWARDDECL_H
#define UTILS_FORWARDDECL_H

#include <cstdint>
#include <unordered_map>
#include <string>

#include <utils/glm/detail/setup.hpp>

namespace simplex
{
namespace utils
{

enum class PixelComponentType : uint16_t;
class Image;

enum class PrimitiveType : uint16_t;
enum class DrawElementsIndexType : uint16_t;
class PrimitiveSet;
class DrawArrays;
class DrawElements;

enum class VertexComponentType : uint16_t;
enum class VertexAttribute : uint16_t;
class Buffer;
class VertexBuffer;
class DrawElementsBuffer;
class Mesh;
class AbstractPainter;
class MeshPainter;

using ShaderDefines = std::unordered_map<std::string, std::string>;
class Shader;

class AbstractClipSpace;
class OrthoClipSpace;
class PerspectiveClipSpace;

class SortedObject;

template<glm::length_t L, typename T> struct TransformT;
using Transform = TransformT<3, float>;

template<glm::length_t L, typename T> struct PlaneT;
using Plane = PlaneT<3, float>;

template <glm::length_t L, typename T> struct BoundingBoxT;
using BoundingBox = BoundingBoxT<3, float>;

template<typename T> struct FrustumT;
using Frustum = FrustumT<float>;

template<typename T> struct OpenFrustumT;
using OpenFrustum = OpenFrustumT<float>;

}
}

#endif // UTILS_FORWARDDECL_H
