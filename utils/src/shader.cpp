#include <fstream>
#include <set>

#include <utils/shader.h>
#include <utils/logger.h>

namespace simplex
{
namespace utils
{

std::string Shader::s_version = "450 core";

Shader::Shader()
    : m_data()
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
    static auto readFile = [](const std::filesystem::path &filename, std::string &data) -> bool
    {
        data.clear();

        std::ifstream shaderFile(filename);
        if (!shaderFile.is_open())
            return false;

        shaderFile.seekg(0, std::ios::end);
        const auto size = shaderFile.tellg();
        data.resize(static_cast<std::string::size_type>(size));
        shaderFile.seekg(0, std::ios::beg);
        shaderFile.read(&data[0], size);
        shaderFile.close();

        return true;
    };

    auto absoluteFilename = std::filesystem::absolute(filename);
    auto absoluteDir = absoluteFilename.parent_path();

    auto result = std::make_shared<Shader>();

    if (!readFile(absoluteFilename, result->m_data))
    {
        LOG_ERROR << "Can't open shader file " << absoluteFilename;
        return nullptr;
    }

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
        auto includedFilename = absoluteDir / std::filesystem::path(result->m_data.substr(pos1, pos2-pos1));

        if (includedFiles.count(includedFilename))
        {
            result->m_data.replace(pos, pos2-pos+1, "");
            continue;
        }

        std::string includedData;
        if (!readFile(includedFilename, includedData))
        {
            LOG_ERROR << "Shader file " << absoluteFilename << ": Can't open included file " << includedFilename;
            return nullptr;
        }

        result->m_data.replace(pos, pos2-pos+1, includedData);
        includedFiles.insert(includedFilename);
    }

    for (const auto &define : defines)
        result->m_data.insert(0, "#define " + define.first + " (" + define.second + ")\n");

    result->m_data.insert(0, versionString + " " + s_version + "\n\n");

//    LOG_INFO << filename;
//    LOG_INFO << result->m_data;

    return result;
}

}
}
