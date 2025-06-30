#ifndef UTILS_FILEEXTENSION_H
#define UTILS_FILEEXTENSION_H

#include <cwctype>
#include <filesystem>

namespace simplex
{
namespace utils
{

inline std::wstring fileExtension(const std::filesystem::path &filename)
{
    auto extension = filename.extension().wstring();
    for (auto &c : extension)
        c = std::towlower(c);
    return extension;
}

}
}

#endif // UTILS_FILEEXTENSION_H
