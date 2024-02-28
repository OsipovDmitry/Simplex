#include <set>

#include <utils/shader.h>
#include <utils/logger.h>
#include <utils/textfile.h>

#define DEBUG_PRINT_SHADER 1

namespace simplex
{
namespace utils
{

std::string Shader::s_version = "450 core";

Shader::Shader()
{
}

Shader::~Shader() = default;

const std::string &Shader::data() const
{
    return m_data;
}

const std::string &Shader::version()
{
    return s_version;
}

void Shader::setVersion(const std::string &value)
{
    s_version = value;
}

std::shared_ptr<Shader> Shader::loadFromData(const std::string &data)
{
    auto result = std::make_shared<Shader>();
    result->m_data = data;
    return result;
}

std::shared_ptr<Shader> Shader::loadFromFile(const std::filesystem::path &filename, const ShaderDefines &defines)
{
    auto absoluteFilename = std::filesystem::absolute(filename);
    auto absoluteDir = absoluteFilename.parent_path();

    auto shaderFile = TextFile::loadFromFile(absoluteFilename);
    if (!shaderFile)
    {
        LOG_ERROR << "Can't open shader file " << absoluteFilename;
        return nullptr;
    }

    auto result = std::make_shared<Shader>();
    result->m_data = std::move(shaderFile->data());

    static const std::string versionString = "#version";
    static const std::string includeString = "#include<";
    static const std::string closedBracket = ">";

    std::set<std::filesystem::path> includedFiles;
    includedFiles.insert(absoluteFilename);

    for (auto pos = result->m_data.find(includeString); pos != std::string::npos; pos = result->m_data.find(includeString, pos))
    {
        auto pos2 = result->m_data.find(closedBracket, pos);
        if (pos2 == std::string::npos)
        {
            LOG_ERROR << "Shader file " << absoluteFilename << ": Wrong order '<' and '>' in #include";
            return nullptr;
        }

        auto pos1 = pos + includeString.length();
        auto includedPath = std::filesystem::path(result->m_data.substr(pos1, pos2-pos1));
        std::filesystem::path includedFilename = includedPath.is_absolute() ?
                    includedPath : absoluteDir / includedPath;

        if (includedFiles.count(includedFilename))
        {
            result->m_data.replace(pos, pos2-pos+1, "");
            continue;
        }

        auto includedFile = TextFile::loadFromFile(includedFilename);
        if (!includedFile)
        {
            LOG_ERROR << "Shader file " << absoluteFilename << ": Can't open included file " << includedFilename;
            return nullptr;
        }

        result->m_data.replace(pos, pos2-pos+1, includedFile->data());
        includedFiles.insert(includedFilename);
    }

    for (const auto &define : defines)
        result->m_data.insert(0, "#define " + define.first + " (" + define.second + ")\n");

    result->m_data.insert(0, versionString + " " + s_version + "\n\n");

#if DEBUG_PRINT_SHADER
    TextFile::loadFromData(result->m_data)->saveToFile(std::filesystem::path(filename.filename()));
#endif

    return result;
}

}
}
