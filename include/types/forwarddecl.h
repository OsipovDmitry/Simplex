#ifndef TYPES_FORWARDDECL_H
#define TYPES_FORWARDDECL_H

#include <memory>

namespace types {

enum class ImageFormat : int32_t;
enum class ImageType : int32_t;

enum class PrimitiveType : int32_t;
enum class IndexType : int32_t;

struct VertexArray;
using VertexArrayPtr = std::shared_ptr<VertexArray>;

struct IndexArray;
using IndexArrayPtr = std::shared_ptr<IndexArray>;

struct Geometry;
using GemetryPtr = std::shared_ptr<Geometry>;

enum class DepthTestFunc : int32_t;
enum class BlendFunc : int32_t;
enum class BlendEquation : int32_t;

}

#endif // TYPES_FORWARDDECL_H
