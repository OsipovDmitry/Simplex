#ifndef CORE_DYNAMICBODYNODEPRIVATE_H
#define CORE_DYNAMICBODYNODEPRIVATE_H

#include "bodynodeprivate.h"

namespace simplex
{
namespace core
{

class DynamicBodyNodePrivate : public BodyNodePrivate
{
public:
    DynamicBodyNodePrivate(DynamicBodyNode&, const std::string&);
    ~DynamicBodyNodePrivate() override;

    std::shared_ptr<physics::IBody> body() override;

    std::shared_ptr<physics::IDynamicBody>& dynamicBody();

    void resetBodyTransformChangedAfterSimulation();

private:
    std::shared_ptr<physics::IDynamicBody> m_dynamicBody;
};

} // namespace core
} // namespace simplex

#endif // CORE_DYNAMICBODYNODEPRIVATE_H
