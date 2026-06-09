#ifndef CORE_STATICBODYNODEPRIVATE_H
#define CORE_STATICBODYNODEPRIVATE_H

#include "bodynodeprivate.h"

namespace simplex
{
namespace core
{

class StaticBodyNodePrivate : public BodyNodePrivate
{
public:
    StaticBodyNodePrivate(StaticBodyNode&, const std::string&);
    ~StaticBodyNodePrivate() override;

    std::shared_ptr<physics::IBody> body() override;

    std::shared_ptr<physics::IStaticBody>& staticBody();

private:
    std::shared_ptr<physics::IStaticBody> m_staticBody;
};

} // namespace core
} // namespace simplex

#endif // CORE_STATICBODYNODEPRIVATE_H
