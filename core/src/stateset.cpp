#include <core/stateset.h>

#include "statesetprivate.h"

namespace simplex
{
namespace core
{

StateSet::StateSet()
    : StateSet(std::make_unique<StateSetPrivate>())
{
}

StateSet::~StateSet() = default;

const UniformCollection &StateSet::uniformCollection() const
{
    return m_->uniformCollection();
}

PConstAbstractUniform StateSet::uniform(UniformID ID) const
{
    return const_cast<StateSet*>(this)->uniform(ID);
}

PAbstractUniform StateSet::uniform(UniformID ID)
{
    auto it = m_->uniformCollection().find(ID);
    return it != m_->uniformCollection().end() ? it->second : nullptr;
}

PAbstractUniform &StateSet::getOrCreateUniform(UniformID ID)
{
    auto it = m_->uniformCollection().find(ID);
    return it != m_->uniformCollection().end() ? it->second : (m_->uniformCollection()[ID] = PAbstractUniform());
}

void StateSet::removeUniform(UniformID ID)
{
    m_->uniformCollection().erase(ID);
}

const UserUniformCollection &StateSet::userUniformCollection() const
{
    return m_->userUniformCollection();
}

PConstAbstractUniform StateSet::userUniform(const std::string& name) const
{
    return const_cast<StateSet*>(this)->userUniform(name);
}

PAbstractUniform StateSet::userUniform(const std::string& name)
{
    auto it = m_->userUniformCollection().find(name);
    return it != m_->userUniformCollection().end() ? it->second : nullptr;
}

PAbstractUniform &StateSet::getOrCreateUserUniform(const std::string& name)
{
    auto it = m_->userUniformCollection().find(name);
    return it != m_->userUniformCollection().end() ? it->second : (m_->userUniformCollection()[name] = PAbstractUniform());
}

void StateSet::removeUserUniform(const std::string &name)
{
    m_->userUniformCollection().erase(name);
}

const UniformBlockCollection& StateSet::uniformBlockCollection() const
{
    return m_->uniformBlockCollection();
}

graphics::PConstBufferRange StateSet::uniformBlock(UniformBlockID ID) const
{
    auto it = m_->uniformBlockCollection().find(ID);
    return it != m_->uniformBlockCollection().end() ? it->second : nullptr;
}

graphics::PConstBufferRange& StateSet::getOrCreateUniformBlock(UniformBlockID ID)
{
    auto it = m_->uniformBlockCollection().find(ID);
    return it != m_->uniformBlockCollection().end() ? it->second : (m_->uniformBlockCollection()[ID] = graphics::PConstBufferRange());
}

void StateSet::removeUniformBlock(UniformBlockID ID)
{
    m_->uniformBlockCollection().erase(ID);
}

const ShaderStorageBlockCollection &StateSet::shaderStorageBlockCollection() const
{
    return m_->shaderStorageBlockCollection();
}

graphics::PConstBufferRange StateSet::shaderStorageBlock(ShaderStorageBlockID ID) const
{
    auto it = m_->shaderStorageBlockCollection().find(ID);
    return it != m_->shaderStorageBlockCollection().end() ? it->second : nullptr;
}

graphics::PConstBufferRange &StateSet::getOrCreateShaderStorageBlock(ShaderStorageBlockID ID)
{
    auto it = m_->shaderStorageBlockCollection().find(ID);
    return it != m_->shaderStorageBlockCollection().end() ? it->second : (m_->shaderStorageBlockCollection()[ID] = graphics::PConstBufferRange());
}

void StateSet::removeShaderStorageBlock(ShaderStorageBlockID ID)
{
    m_->shaderStorageBlockCollection().erase(ID);
}

StateSet::StateSet(std::unique_ptr<StateSetPrivate> stateSetPrivate)
    : m_(std::move(stateSetPrivate))
{
}

}
}
