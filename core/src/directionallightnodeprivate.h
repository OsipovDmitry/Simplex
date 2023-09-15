#ifndef CORE_DIRECTIONALLIGHTNODEPRIVATE_H
#define CORE_DIRECTIONALLIGHTNODEPRIVATE_H

#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

class DirectionalLightNodePrivate : public LightNodePrivate
{
public:
    DirectionalLightNodePrivate(const std::string &name);
    ~DirectionalLightNodePrivate() override;

private:
};

}
}

#endif // CORE_DIRECTIONALLIGHTNODEPRIVATE_H
