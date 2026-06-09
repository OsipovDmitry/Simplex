#ifndef CORE_BODYNODEPRIVATE_H
#define CORE_BODYNODEPRIVATE_H

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

class BodyNodePrivate : public NodePrivate
{
public:
    BodyNodePrivate(BodyNode&, const std::string&);
    ~BodyNodePrivate() override;

    void onAfterTransformChanged() override;
    void onAttachToScene(const std::shared_ptr<Scene>&) override;
    void onDetachFromScene(const std::shared_ptr<Scene>&) override;

    virtual std::shared_ptr<physics::IBody> body() = 0;
};

} // namespace core
} // namespace simplex

#endif // CORE_BODYNODEPRIVATE_H
