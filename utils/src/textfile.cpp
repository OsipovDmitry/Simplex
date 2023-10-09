#include <fstream>

#include <utils/textfile.h>

namespace simplex
{
namespace utils
{

TextFile::TextFile()
{
}

TextFile::~TextFile() = default;

const std::string &TextFile::data() const
{
    return const_cast<TextFile*>(this)->data();
}

std::string &TextFile::data()
{
    return m_data;
}

std::shared_ptr<TextFile> TextFile::loadFromFile(const std::filesystem::path &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
        return nullptr;

    auto result = std::make_shared<TextFile>();

    file.seekg(0, std::ios::end);
    const auto size = file.tellg();
    result->m_data.resize(static_cast<std::string::size_type>(size));
    file.seekg(0, std::ios::beg);
    file.read(&result->m_data[0], size);
    file.close();

    return result;
}

std::shared_ptr<TextFile> TextFile::loadFromData(const std::string &data)
{
    auto result = std::make_shared<TextFile>();
    result->data() = data;
    return result;
}

bool TextFile::saveToFile(const std::filesystem::path &filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
        return false;

    file.write(m_data.data(), static_cast<std::streamsize>(m_data.size()));
    file.close();

    return true;
}

}
}
