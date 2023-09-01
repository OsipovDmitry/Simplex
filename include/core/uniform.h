#ifndef UNIFORM_H
#define UNIFORM_H

#include <core/igraphicsrenderer.h>

namespace simplex
{
namespace core
{

template<typename T> inline graphics::UniformType toUniformType() { return graphics::UniformType::Undefined; }

class AbstractUniform
{
public:
    virtual ~AbstractUniform() = default;
    virtual graphics::UniformType type() const = 0;
};

template<typename T>
class UniformBase : public AbstractUniform
{
public:
    UniformBase(const T &value) : m_data(value) {}

    const T &data() const { return m_data; }
    T &data() { return m_data; }

protected:
    T m_data;
};

template<typename T>
class Uniform : public UniformBase<T>
{
public:
    Uniform(const T &value) : UniformBase<T>(value) {}
    graphics::UniformType type() const override { return toUniformType<T>(); }
};

template<>
class Uniform<graphics::PTexture> : public UniformBase<graphics::PTexture>
{
public:
    Uniform(const graphics::PTexture &value) : UniformBase<graphics::PTexture>(value) {}
    graphics::UniformType type() const override { return graphics::IRenderProgram::uniformTypeByTextureType(m_data->type()); }
};

template<>
class Uniform<graphics::PImage> : public UniformBase<graphics::PImage>
{
public:
    Uniform(const graphics::PImage &value) : UniformBase<graphics::PImage>(value) {}
    graphics::UniformType type() const override { return graphics::IRenderProgram::uniformTypeByImageTextureType(m_data->texture()->type()); }
};

template<>
class Uniform<graphics::PBufferRange> : public UniformBase<graphics::PBufferRange>
{
public:
    Uniform(const graphics::PBufferRange &value) : UniformBase<graphics::PBufferRange>(value) {}
    graphics::UniformType type() const override { return core::graphics::UniformType::AtomicCounterUint; }
};

template<> inline graphics::UniformType toUniformType<float>() { return graphics::UniformType::Single; }
template<> inline graphics::UniformType toUniformType<glm::vec2>() { return graphics::UniformType::SingleVec2; }
template<> inline graphics::UniformType toUniformType<glm::vec3>() { return graphics::UniformType::SingleVec3; }
template<> inline graphics::UniformType toUniformType<glm::vec4>() { return graphics::UniformType::SingleVec4; }
template<> inline graphics::UniformType toUniformType<glm::mat2x2>() { return graphics::UniformType::SingleMat2; }
template<> inline graphics::UniformType toUniformType<glm::mat3x3>() { return graphics::UniformType::SingleMat3; }
template<> inline graphics::UniformType toUniformType<glm::mat4x4>() { return graphics::UniformType::SingleMat4; }
template<> inline graphics::UniformType toUniformType<double>() { return graphics::UniformType::Double; }
template<> inline graphics::UniformType toUniformType<glm::dvec2>() { return graphics::UniformType::DoubleVec2; }
template<> inline graphics::UniformType toUniformType<glm::dvec3>() { return graphics::UniformType::DoubleVec3; }
template<> inline graphics::UniformType toUniformType<glm::dvec4>() { return graphics::UniformType::DoubleVec4; }
template<> inline graphics::UniformType toUniformType<glm::dmat2x2>() { return graphics::UniformType::DoubleMat2; }
template<> inline graphics::UniformType toUniformType<glm::dmat3x3>() { return graphics::UniformType::DoubleMat3; }
template<> inline graphics::UniformType toUniformType<glm::dmat4x4>() { return graphics::UniformType::DoubleMat4; }
template<> inline graphics::UniformType toUniformType<int32_t>() { return graphics::UniformType::Int32; }
template<> inline graphics::UniformType toUniformType<glm::i32vec2>() { return graphics::UniformType::Int32Vec2; }
template<> inline graphics::UniformType toUniformType<glm::i32vec3>() { return graphics::UniformType::Int32Vec3; }
template<> inline graphics::UniformType toUniformType<glm::i32vec4>() { return graphics::UniformType::Int32Vec4; }
template<> inline graphics::UniformType toUniformType<uint32_t>() { return graphics::UniformType::Uint32; }
template<> inline graphics::UniformType toUniformType<glm::u32vec2>() { return graphics::UniformType::Uint32Vec2; }
template<> inline graphics::UniformType toUniformType<glm::u32vec3>() { return graphics::UniformType::Uint32Vec3; }
template<> inline graphics::UniformType toUniformType<glm::u32vec4>() { return graphics::UniformType::Uint32Vec4; }


template<typename T>
PUniform<T> makeUniform(const T &value) { return std::make_shared<Uniform<T>>(value); }

template <typename T>
std::shared_ptr<Uniform<T>> uniform_cast(const std::shared_ptr<AbstractUniform> &uniform)
{
    return std::dynamic_pointer_cast<Uniform<T>>(uniform);
}

template <typename T>
std::shared_ptr<const Uniform<T>> uniform_cast(const std::shared_ptr<const AbstractUniform> &uniform)
{
    return std::dynamic_pointer_cast<const Uniform<T>>(uniform);
}

}
}

#endif // UNIFORM_H
