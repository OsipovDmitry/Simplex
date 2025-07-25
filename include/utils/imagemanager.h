#ifndef UTILS_IMAGEMANAGER_H
#define UTILS_IMAGEMANAGER_H

#include <memory>
#include <filesystem>
#include <unordered_map>

#include <utils/utilsglobal.h>
#include <utils/noncopyble.h>
#include <utils/singleton.h>
#include <utils/forwarddecl.h>

namespace simplex
{
namespace utils
{

class UTILS_SHARED_EXPORT ImageManager final
{
    NONCOPYBLE(ImageManager)
    SINGLETON(ImageManager)
public:
    ~ImageManager();


    std::shared_ptr<Image> loadOrGet(const std::filesystem::path&);

private:
    ImageManager();

    std::unordered_map<std::filesystem::path, std::shared_ptr<Image>> m_images;
};

}
}

#endif // UTILS_IMAGEMANAGER_H
