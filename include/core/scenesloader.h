#ifndef CORE_SCENESLOADER_H
#define CORE_SCENESLOADER_H

#include <core/coreglobal.h>
#include <core/forwarddecl.h>
#include <core/resourceloader.h>

namespace simplex
{
namespace core
{

class ScenesLoaderPrivate;
class CORE_SHARED_EXPORT ScenesLoader : public ResourceLoaderBase<SceneRepresentation>
{
    NONCOPYBLE(ScenesLoader)
public:
    ScenesLoader(const std::string&);
    ~ScenesLoader() override;

private:
    std::unique_ptr<ScenesLoaderPrivate> m_;
};

} // namespace core
} // namespace simplex

#endif // CORE_SCENESLOADER_H
