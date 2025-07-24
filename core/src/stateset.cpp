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

PConstAbstractUniform StateSet::uniform(UniformId id) const
{
    return const_cast<StateSet*>(this)->uniform(id);
}

PAbstractUniform StateSet::uniform(UniformId id)
{
    auto it = m_->uniformCollection().find(id);
    return it != m_->uniformCollection().end() ? it->second : nullptr;
}

PAbstractUniform &StateSet::getOrCreateUniform(UniformId id)
{
    auto it = m_->uniformCollection().find(id);
    return it != m_->uniformCollection().end() ? it->second : (m_->uniformCollection()[id] = PAbstractUniform());
}

void StateSet::removeUniform(UniformId id)
{
    m_->uniformCollection().erase(id);
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

const SSBOCollection &StateSet::ssboCollection() const
{
    return m_->ssboCollection();
}

graphics::PConstBuffer StateSet::SSBO(SSBOId id) const
{
    auto it = m_->ssboCollection().find(id);
    return it != m_->ssboCollection().end() ? it->second : nullptr;
}

graphics::PConstBuffer &StateSet::getOrCreateSSBO(SSBOId id)
{
    auto it = m_->ssboCollection().find(id);
    return it != m_->ssboCollection().end() ? it->second : (m_->ssboCollection()[id] = graphics::PConstBuffer());
}

void StateSet::removeSSBO(SSBOId id)
{
    m_->ssboCollection().erase(id);
}

StateSet::StateSet(std::unique_ptr<StateSetPrivate> stateSetPrivate)
    : m_(std::move(stateSetPrivate))
{
}

}
}
