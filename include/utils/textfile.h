#ifndef UTILS_TEXTFILE_H
#define UTILS_TEXTFILE_H

#include <memory>
#include <string>
#include <filesystem>

#include <utils/utilsglobal.h>
#include <utils/noncopyble.h>
#include <utils/forwarddecl.h>

namespace simplex
{
namespace utils
{

class UTILS_SHARED_EXPORT TextFile final
{
    NONCOPYBLE(TextFile)

public:
    TextFile();
    ~TextFile();

    const std::string &data() const;
    std::string &data();

    static std::shared_ptr<TextFile> loadFromFile(const std::filesystem::path&);
    static std::shared_ptr<TextFile> loadFromData(const std::string&);
    bool saveToFile(const std::filesystem::path&);

private:
    std::string m_data;

};

}
}

#endif // UTILS_TEXTFILE_H
