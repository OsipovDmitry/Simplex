#include <set>
#include <optional>

#include <utils/shader.h>
#include <utils/logger.h>
#include <utils/textfile.h>

namespace simplex
{
namespace utils
{

std::string Shader::s_version = "460 core";
bool Shader::s_printDebugShaders = false;

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

bool Shader::printDebugShaders()
{
    return s_printDebugShaders;
}

void Shader::setPrintDebugShaders(bool value)
{
    s_printDebugShaders = value;
}

std::shared_ptr<Shader> Shader::loadFromData(const std::string &data)
{
    auto result = std::make_shared<Shader>();
    result->m_data = data;
    return result;
}

std::shared_ptr<Shader> Shader::loadFromFile(const std::filesystem::path &filename, const ShaderDefines &defines)
{
    static const std::string versionString = "#version";
    static const std::string includeString = "#include<";
    static const std::string closedBracket = ">";

    static const auto includeProccess = [](
        const auto& self,
        const std::string& srcData,
        const std::filesystem::path& filename,
        const std::filesystem::path& dir,
        std::set<std::filesystem::path>& includedFiles) -> std::optional<std::string>
    {
        std::string result = srcData;

        for (auto pos = result.find(includeString); pos != std::string::npos; pos = result.find(includeString, pos))
        {
            auto pos2 = result.find(closedBracket, pos);
            if (pos2 == std::string::npos)
            {
                LOG_ERROR << "Shader file " << filename << ": Wrong order '<' and '>' in #include";
                return std::nullopt;
            }

            auto pos1 = pos + includeString.length();
            auto includedPath = std::filesystem::path(result.substr(pos1, pos2 - pos1));
            std::filesystem::path includedFilename = includedPath.is_absolute() ?
                includedPath :
                dir / includedPath;
            includedFilename = std::filesystem::canonical(includedFilename);

            if (includedFiles.count(includedFilename))
            {
                result.replace(pos, pos2 - pos + 1, "");
                continue;
            }
            includedFiles.insert(includedFilename);

            auto includedFile = TextFile::loadFromFile(includedFilename);
            if (!includedFile)
            {
                LOG_ERROR << "Shader file " << filename << ": Can't open included file " << includedFilename;
                return std::nullopt;
            }

            auto includedData = self(self, includedFile->data(), includedPath, includedFilename.parent_path(), includedFiles);
            if (!includedData.has_value())
            {
                return std::nullopt;
            }

            result.replace(pos, pos2 - pos + 1, includedData.value());
        }

        return result;
    };

    auto absoluteFilename = std::filesystem::absolute(filename);

    auto shaderFile = TextFile::loadFromFile(absoluteFilename);
    if (!shaderFile)
    {
        LOG_ERROR << "Can't open shader file " << absoluteFilename;
        return nullptr;
    }

    auto result = std::make_shared<Shader>();

    std::set<std::filesystem::path> includedFiles;
    includedFiles.insert(std::filesystem::canonical(absoluteFilename));

    if (auto includedData = includeProccess(
        includeProccess,
        shaderFile->data(),
        absoluteFilename,
        absoluteFilename.parent_path(),
        includedFiles);
        !includedData.has_value())
    {
        LOG_ERROR << "Shader file " << absoluteFilename << " parse error";
        return nullptr;
    }
    else
        result->m_data = std::move(includedData.value());

    for (const auto &define : defines)
        result->m_data.insert(0, "#define " + define.first + " " + define.second + "\n");

    result->m_data.insert(0, versionString + " " + s_version + "\n\n");

    if (s_printDebugShaders)
        TextFile::loadFromData(result->m_data)->saveToFile(std::filesystem::path(filename.filename()));

    return result;
}

}
}
