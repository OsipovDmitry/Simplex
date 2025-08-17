#ifndef UNIFORM_H
#define UNIFORM_H

#include <core/graphicsrendererbase.h>

namespace simplex
{
namespace core
{

template<typename T>
using remove_cvr = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

class AbstractUniform
{
public:
    virtual ~AbstractUniform() = default;
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
};

template<typename T>
PUniform<T> makeUniform(const T &value) {
    return std::make_shared<Uniform<remove_cvr<T>>>(value);
}

template <typename T>
std::shared_ptr<Uniform<T>> uniform_cast(const PAbstractUniform &uniform)
{
    return std::dynamic_pointer_cast<Uniform<T>>(uniform);
}

template <typename T>
std::shared_ptr<const Uniform<T>> uniform_cast(const PConstAbstractUniform &uniform)
{
    return std::dynamic_pointer_cast<const Uniform<T>>(uniform);
}

}
}

#endif // UNIFORM_H
