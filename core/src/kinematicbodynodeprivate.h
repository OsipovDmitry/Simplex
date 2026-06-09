#ifndef CORE_KINEMATICBODYNODEPRIVATE_H
#define CORE_KINEMATICBODYNODEPRIVATE_H

#include "bodynodeprivate.h"

namespace simplex
{
namespace core
{

class KinematicBodyNodePrivate : public BodyNodePrivate
{
public:
    KinematicBodyNodePrivate(KinematicBodyNode&, const std::string&);
    ~KinematicBodyNodePrivate() override;

    std::shared_ptr<physics::IBody> body() override;

    std::shared_ptr<physics::IKinematicBody>& kinematicBody();

private:
    std::shared_ptr<physics::IKinematicBody> m_kinematicBody;
};

} // namespace core
} // namespace simplex

#endif // CORE_KINEMATICBODYNODEPRIVATE_H
