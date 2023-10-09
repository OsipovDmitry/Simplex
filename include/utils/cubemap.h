#ifndef UTILS_CUBEMAP_H
#define UTILS_CUBEMAP_H

#include <memory>
#include <vector>

#include <utils/utilsglobal.h>
#include <utils/forwarddecl.h>
#include <utils/noncopyble.h>

namespace simplex
{
namespace utils
{

class SIMPEX_DECL_EXPORT CubemapConverter
{
    NONCOPYBLE(CubemapConverter)

public:
    explicit CubemapConverter() = delete;

    static std::shared_ptr<Image> convertSphericalCubemapToHCross(const std::shared_ptr<Image>&);
    static std::shared_ptr<Image> convertSphericalCubemapToVCross(const std::shared_ptr<Image>&);
    static std::vector<std::shared_ptr<Image>> convertSphericalCubemapToFacesList(const std::shared_ptr<Image>&);

};

}
}

#endif // UTILS_CUBEMAP_H
