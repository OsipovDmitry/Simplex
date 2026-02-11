#include <optional>

#include <utils/rapidjson/document.h>
#include <utils/textfile.h>
#include <utils/fileextension.h>
#include <utils/image.h>

#include <core/graphicsrendererbase.h>
#include <core/texturesloader.h>

#include "resources.h"
#include "texturesloaderprivate.h"

namespace simplex
{
namespace core
{

class TexturesLoaderBase : public LoaderBase<graphics::ITexture>
{
public:
    TexturesLoaderBase(const std::string& name, const std::shared_ptr<graphics::RendererBase>& graphicsRenderer)
        : LoaderBase<graphics::ITexture>(name)
        , m_graphicsRenderer(graphicsRenderer)
    {}

    ~TexturesLoaderBase() override = default;

protected:
    std::shared_ptr<graphics::RendererBase> m_graphicsRenderer;
};


class StandrdTexturesLoader : public TexturesLoaderBase
{
public:
    StandrdTexturesLoader(const std::string& name, const std::shared_ptr<graphics::RendererBase>& graphicsRenderer)
        : TexturesLoaderBase(name, graphicsRenderer)
    {}

    ~StandrdTexturesLoader() override = default;

    std::shared_ptr<graphics::ITexture> loadResource(const std::filesystem::path& absoluteFileName) const override
    {
        std::shared_ptr<graphics::ITexture> texture;
        if (auto image = utils::Image::loadFromFile(absoluteFileName))
        {
            m_graphicsRenderer->makeCurrent();
            texture = m_graphicsRenderer->createTexture2D(image);
        }
        return texture;
    }
};

class JSONTexturesLoader : public TexturesLoaderBase
{
public:
    JSONTexturesLoader(const std::string& name, const std::shared_ptr<graphics::RendererBase>& graphicsRenderer)
        : TexturesLoaderBase(name, graphicsRenderer)
    {}

    ~JSONTexturesLoader() override = default;

    std::shared_ptr<graphics::ITexture> loadResource(const std::filesystem::path& absoluteFileName) const override
    {
        std::shared_ptr<graphics::ITexture> texture;
        if (utils::fileExtension(absoluteFileName) == L".json")
        {
            auto textureFile = utils::TextFile::loadFromFile(absoluteFileName);
            if (textureFile)
            {
                rapidjson::Document document;
                document.Parse(textureFile->data().c_str());
                if (document.IsObject())
                {
                    m_graphicsRenderer->makeCurrent();
                    texture = loadTextureFromJSON(m_graphicsRenderer, document, absoluteFileName.parent_path());
                }
                else
                    LOG_ERROR << "Root node of texture description file " << absoluteFileName << " must be object";
            }
        }
        return texture;
    }

private:
    using Images2D = std::vector<std::filesystem::path>;
    using Images3D = std::vector<Images2D>;
    using ImagesCube = std::array<Images2D, 6u>;

    static graphics::TextureType string2TextureType(const std::string& value)
    {
        static const std::unordered_map<std::string, graphics::TextureType> s_table{
            { "1D", graphics::TextureType::Type1D },
            { "2D", graphics::TextureType::Type2D },
            { "3D", graphics::TextureType::Type3D },
            { "Cube", graphics::TextureType::TypeCube },
            { "1DArray", graphics::TextureType::Type1DArray },
            { "2DArray", graphics::TextureType::Type2DArray },
            { "CubeArray", graphics::TextureType::TypeCubeArray },
            { "Rect", graphics::TextureType::TypeRect },
        };

        auto it = s_table.find(value);
        return (it == s_table.end()) ? graphics::TextureType::Count : it->second;
    }

    static graphics::TextureWrapMode string2TextureWrapMode(const std::string& value)
    {
        static const std::unordered_map<std::string, graphics::TextureWrapMode> s_table{
            { "ClampToEdge", graphics::TextureWrapMode::ClampToEdge },
            { "ClampToBorder", graphics::TextureWrapMode::ClampToBorder },
            { "MirroredRepeat", graphics::TextureWrapMode::MirroredRepeat },
            { "Repeat", graphics::TextureWrapMode::Repeat },
            { "MirrorClampToEdge", graphics::TextureWrapMode::MirrorClampToEdge },
        };

        auto it = s_table.find(value);
        return (it == s_table.end()) ? graphics::TextureWrapMode::Count : it->second;
    }

    static graphics::TextureFilterMode string2TextureFilterMode(const std::string& value)
    {
        static const std::unordered_map<std::string, graphics::TextureFilterMode> s_table{
            { "Point", graphics::TextureFilterMode::Point },
            { "Linear", graphics::TextureFilterMode::Linear },
            { "Bilinear", graphics::TextureFilterMode::Bilinear },
            { "Trilinear", graphics::TextureFilterMode::Trilinear },
        };

        auto it = s_table.find(value);
        return (it == s_table.end()) ? graphics::TextureFilterMode::Count : it->second;
    }

    static graphics::PixelInternalFormat string2PixelInternalFormat(const std::string& value)
    {
        static const std::unordered_map<std::string, graphics::PixelInternalFormat> s_table{
            { "R8", graphics::PixelInternalFormat::R8 },
            { "R8_SNORM", graphics::PixelInternalFormat::R8_SNORM },
            { "R16", graphics::PixelInternalFormat::R16 },
            { "R16_SNORM", graphics::PixelInternalFormat::R16_SNORM },
            { "RG8", graphics::PixelInternalFormat::RG8 },
            { "RG8_SNORM", graphics::PixelInternalFormat::RG8_SNORM },
            { "RG16", graphics::PixelInternalFormat::RG16 },
            { "RG16_SNORM", graphics::PixelInternalFormat::RG16_SNORM },
            { "R3_G3_B2", graphics::PixelInternalFormat::R3_G3_B2 },
            { "RGB4", graphics::PixelInternalFormat::RGB4 },
            { "RGB5", graphics::PixelInternalFormat::RGB5 },
            { "RGB8", graphics::PixelInternalFormat::RGB8 },
            { "RGB8_SNORM", graphics::PixelInternalFormat::RGB8_SNORM },
            { "RGB10", graphics::PixelInternalFormat::RGB10 },
            { "RGB12", graphics::PixelInternalFormat::RGB12 },
            { "RGB16", graphics::PixelInternalFormat::RGB16 },
            { "RGB16_SNORM", graphics::PixelInternalFormat::RGB16_SNORM },
            { "RGBA2", graphics::PixelInternalFormat::RGBA2 },
            { "RGBA4", graphics::PixelInternalFormat::RGBA4 },
            { "RGB5_A1", graphics::PixelInternalFormat::RGB5_A1 },
            { "RGBA8", graphics::PixelInternalFormat::RGBA8 },
            { "RGBA8_SNORM", graphics::PixelInternalFormat::RGBA8_SNORM },
            { "RGB10_A2", graphics::PixelInternalFormat::RGB10_A2 },
            { "RGB10_A2UI", graphics::PixelInternalFormat::RGB10_A2UI },
            { "RGBA12", graphics::PixelInternalFormat::RGBA12 },
            { "RGBA16", graphics::PixelInternalFormat::RGBA16 },
            { "RGBA16_SNORM", graphics::PixelInternalFormat::RGBA16_SNORM },
            { "SRGB8", graphics::PixelInternalFormat::SRGB8 },
            { "SRGB8_ALPHA8", graphics::PixelInternalFormat::SRGB8_ALPHA8 },
            { "R16F", graphics::PixelInternalFormat::R16F },
            { "RG16F", graphics::PixelInternalFormat::RG16F },
            { "RGB16F", graphics::PixelInternalFormat::RGB16F },
            { "RGBA16F", graphics::PixelInternalFormat::RGBA16F },
            { "R32F", graphics::PixelInternalFormat::R32F },
            { "RG32F", graphics::PixelInternalFormat::RG32F },
            { "RGB32F", graphics::PixelInternalFormat::RGB32F },
            { "RGBA32F", graphics::PixelInternalFormat::RGBA32F },
            { "R11F_G11F_B10F", graphics::PixelInternalFormat::R11F_G11F_B10F },
            { "RGB9_E5", graphics::PixelInternalFormat::RGB9_E5 },
            { "R8I", graphics::PixelInternalFormat::R8I },
            { "R8UI", graphics::PixelInternalFormat::R8UI },
            { "R16I", graphics::PixelInternalFormat::R16I },
            { "R16UI", graphics::PixelInternalFormat::R16UI },
            { "R32I", graphics::PixelInternalFormat::R32I },
            { "R32UI", graphics::PixelInternalFormat::R32UI },
            { "RG8I", graphics::PixelInternalFormat::RG8I },
            { "RG8UI", graphics::PixelInternalFormat::RG8UI },
            { "RG16I", graphics::PixelInternalFormat::RG16I },
            { "RG16UI", graphics::PixelInternalFormat::RG16UI },
            { "RG32I", graphics::PixelInternalFormat::RG32I },
            { "RG32UI", graphics::PixelInternalFormat::RG32UI },
            { "RGB8I", graphics::PixelInternalFormat::RGB8I },
            { "RGB8UI", graphics::PixelInternalFormat::RGB8UI },
            { "RGB16I", graphics::PixelInternalFormat::RGB16I },
            { "RGB16UI", graphics::PixelInternalFormat::RGB16UI },
            { "RGB32I", graphics::PixelInternalFormat::RGB32I },
            { "RGB32UI", graphics::PixelInternalFormat::RGB32UI },
            { "RGBA8I", graphics::PixelInternalFormat::RGBA8I },
            { "RGBA8UI", graphics::PixelInternalFormat::RGBA8UI },
            { "RGBA16I", graphics::PixelInternalFormat::RGBA16I },
            { "RGBA16UI", graphics::PixelInternalFormat::RGBA16UI },
            { "RGBA32I", graphics::PixelInternalFormat::RGBA32I },
            { "RGBA32UI", graphics::PixelInternalFormat::RGBA32UI },
            { "Depth16", graphics::PixelInternalFormat::Depth16 },
            { "Depth24", graphics::PixelInternalFormat::Depth24 },
            { "Depth32F", graphics::PixelInternalFormat::Depth32F },
            { "Stencil8", graphics::PixelInternalFormat::Stencil8 },
            { "Depth24Stencil8", graphics::PixelInternalFormat::Depth24Stencil8 },
            { "Dept32FStencil8", graphics::PixelInternalFormat::Depth32FStencil8 },
        };

        auto it = s_table.find(value);
        return (it == s_table.end()) ? graphics::PixelInternalFormat::Count : it->second;
    }

    static bool loadImage(const rapidjson::Value& value, std::filesystem::path& image)
    {
        if (!value.IsString())
        {
            LOG_ERROR << "Data path field must be string";
            return false;
        }

        image = value.GetString();
        return true;
    }

    static bool loadImages2D(const rapidjson::Value& value, Images2D& images)
    {
        if (value.IsArray())
        {
            images.resize(static_cast<size_t>(value.Size()));
            for (rapidjson::SizeType i = 0; i < value.Size(); ++i)
                if (!loadImage(value[i], images[static_cast<size_t>(i)]))
                    return false;
        }
        else if (value.IsString())
        {
            images.resize(1u);
            if (!loadImage(value, images[0u]))
                return false;
        }
        else
        {
            LOG_ERROR << "Data path field must be string or string[]";
            return false;
        }

        return true;
    }

    static bool loadImages3D(const rapidjson::Value& value, Images3D& images)
    {
        if (value.IsArray())
        {
            images.resize(static_cast<size_t>(value.Size()));
            for (rapidjson::SizeType i = 0; i < value.Size(); ++i)
                if (!loadImages2D(value[i], images[static_cast<size_t>(i)]))
                    return false;
        }
        else if (value.IsString())
        {
            images.resize(1u);
            images[0u].resize(1u);
            if (!loadImage(value, images[0u][0u]))
                return false;
        }
        else
        {
            LOG_ERROR << "Data path field must be string, string[] or string[][]";
            return false;
        }

        return true;
    }

    static std::shared_ptr<graphics::ITexture> loadTextureFromJSON(
        const std::shared_ptr<graphics::RendererBase>& graphicsRenderer,
        const rapidjson::Value& document,
        const std::filesystem::path& absoluteDir)
    {
        static const std::string s_typeField = "Type";
        static const std::string s_dataField = "Data";
        static const std::string s_internalFormatField = "InternalFormat";
        static const std::string s_wrapModeField = "WrapMode";
        static const std::string s_filterModeField = "FilterMode";
        static const std::string s_autogenMipmapsField = "AutogenMipmaps";
        static const std::string s_borderColorField = "BorderColor";

        if (!document.HasMember(s_typeField.c_str()))
        {
            LOG_ERROR << "Texture declaration must include field \"" << s_typeField << "\"";
            return nullptr;
        }

        auto& typeField = document[s_typeField.c_str()];
        if (!typeField.IsString())
        {
            LOG_ERROR << "Type field must be string";
            return nullptr;
        }

        auto textureType = string2TextureType(typeField.GetString());
        if (textureType == graphics::TextureType::Count)
        {
            LOG_ERROR << "Type \"" << typeField.GetString() << "\" is undefined";
            return nullptr;
        }

        auto textureInternalFormat = graphics::PixelInternalFormat::Count;
        if (document.HasMember(s_internalFormatField.c_str()))
        {
            auto& internalFormatField = document[s_internalFormatField.c_str()];
            if (!internalFormatField.IsString())
            {
                LOG_ERROR << "Internal format field must be string";
                return nullptr;
            }

            textureInternalFormat = string2PixelInternalFormat(internalFormatField.GetString());
            if (textureInternalFormat == graphics::PixelInternalFormat::Count)
            {
                LOG_ERROR << "Internal format \"" << internalFormatField.GetString() << "\" is undefined";
                return nullptr;
            }
        }

        auto textureAutogenMipmaps = false;
        if (document.HasMember(s_autogenMipmapsField.c_str()))
        {
            auto& autogenMipmapsField = document[s_autogenMipmapsField.c_str()];
            if (!autogenMipmapsField.IsBool())
            {
                LOG_ERROR << "Autogen mipmaps field must be bool";
                return nullptr;
            }

            textureAutogenMipmaps = autogenMipmapsField.GetBool();
        }

        std::optional<graphics::TextureWrapMode> textureWrapMode;
        if (document.HasMember(s_wrapModeField.c_str()))
        {
            auto& wrapModeField = document[s_wrapModeField.c_str()];
            if (!wrapModeField.IsString())
            {
                LOG_ERROR << "Wrap mode field must be string";
                return nullptr;
            }

            textureWrapMode = string2TextureWrapMode(wrapModeField.GetString());
            if (textureWrapMode == graphics::TextureWrapMode::Count)
            {
                LOG_ERROR << "Wrap mode \"" << wrapModeField.GetString() << "\" is undefined";
                return nullptr;
            }
        }

        std::optional<graphics::TextureFilterMode> textureFilterMode;
        if (document.HasMember(s_filterModeField.c_str()))
        {
            auto& filterModeField = document[s_filterModeField.c_str()];
            if (!filterModeField.IsString())
            {
                LOG_ERROR << "Filter mode field must be string";
                return nullptr;
            }

            textureFilterMode = string2TextureFilterMode(filterModeField.GetString());
            if (textureFilterMode == graphics::TextureFilterMode::Count)
            {
                LOG_ERROR << "Filter mode \"" << filterModeField.GetString() << "\" is undefined";
                return nullptr;
            }
        }

        std::optional<glm::vec4> textureBorderColor;
        if (document.HasMember(s_borderColorField.c_str()))
        {
            auto& borderColorField = document[s_borderColorField.c_str()];
            if (!borderColorField.IsArray() || (borderColorField.Size() != 4u))
            {
                LOG_ERROR << "Border color field must be array[4]";
                return nullptr;
            }

            glm::vec4 borderColorValue;
            for (rapidjson::SizeType i = 0; i < 4u; ++i)
            {
                if (!borderColorField[i].IsNumber())
                {
                    LOG_ERROR << "Elemets of border color field must be numbers";
                    return nullptr;
                }
                borderColorValue[static_cast<glm::length_t>(i)] = borderColorField[i].GetFloat();
            }

            textureBorderColor = borderColorValue;
        }

        if (!document.HasMember(s_dataField.c_str()))
        {
            LOG_ERROR << "Texture declaration must include field \"" << s_dataField << "\"";
            return nullptr;
        }

        auto& dataField = document[s_dataField.c_str()];
        std::vector<std::vector<std::shared_ptr<const utils::Image>>> imagesData;

        Images3D imagesFilenames;
        if (loadImages3D(dataField, imagesFilenames))
        {
            imagesData.resize(imagesFilenames.size());
            for (size_t layer = 0; layer < imagesFilenames.size(); ++layer)
            {
                std::vector<std::shared_ptr<const utils::Image>> layerData(imagesFilenames[layer].size());
                for (size_t level = 0; level < imagesFilenames[layer].size(); ++level)
                {
                    const auto& imageFilename = imagesFilenames[layer][level];
                    auto absoluteImagePath = imageFilename.is_absolute() ? imageFilename : absoluteDir / imageFilename;

                    auto image = utils::Image::loadFromFile(absoluteImagePath);
                    if (!image)
                    {
                        LOG_ERROR << "Failed to load iamge " << absoluteImagePath;
                        return nullptr;
                    }
                    layerData[level] = image;
                }
                imagesData[layer] = std::move(layerData);
            }
        }


        if (imagesData.empty())
        {
            LOG_ERROR << "Layers count can't be 0";
            return nullptr;
        }

        if (imagesData[0u].empty())
        {
            LOG_ERROR << "Levels count can't be 0";
            return nullptr;
        }

        const auto layersCount = static_cast<uint32_t>(imagesData.size());
        const auto levelsCount = static_cast<uint32_t>(imagesData[0u].size());
        const auto firstLevelWidth = imagesData[0u][0u]->width();
        const auto firstLevelHeight = imagesData[0u][0u]->height();
        const auto numComponents = imagesData[0u][0u]->numComponents();
        const auto type = imagesData[0u][0u]->type();

        for (const auto& layerData : imagesData)
        {
            if (levelsCount != layerData.size())
            {
                LOG_ERROR << "All the images layers must be the same count of levels";
                return nullptr;
            }

            if ((firstLevelWidth != layerData[0u]->width()) ||
                (firstLevelHeight != layerData[0u]->height()))
            {
                LOG_ERROR << "All the images layers must be the same size";
                return nullptr;
            }

            for (const auto& levelData : layerData)
            {
                if ((numComponents != levelData->numComponents()) ||
                    (type != levelData->type()))
                {
                    LOG_ERROR << "All the images levels must be the same components count and pixel component type";
                    return nullptr;
                }
            }
        }

        std::shared_ptr<graphics::ITexture> result;
        switch (textureType)
        {
        case graphics::TextureType::Type1D:
        {
            if (layersCount != 1u)
            {
                LOG_ERROR << "Images layers count of 1D texture must be 1";
                return nullptr;
            }

            const auto& layerData = imagesData[0];
            result = graphicsRenderer->createTexture1D(layerData[0u],
                textureInternalFormat,
                levelsCount,
                textureAutogenMipmaps);
            if (!result)
            {
                LOG_ERROR << "Failed to create 1D texture";
                return nullptr;
            }

            for (uint32_t level = 1u; level < levelsCount; ++level)
                result->setSubImage(level,
                    glm::uvec3(0u),
                    glm::uvec3(layerData[level]->width(), 0u, 0u),
                    numComponents,
                    type,
                    layerData[level]->data());
            break;
        }
        case graphics::TextureType::Type2D:
        {
            if (layersCount != 1u)
            {
                LOG_ERROR << "Images layers count of 2D texture must be 1";
                return nullptr;
            }

            const auto& layerData = imagesData[0];
            result = graphicsRenderer->createTexture2D(layerData[0u],
                textureInternalFormat,
                levelsCount,
                textureAutogenMipmaps);
            if (!result)
            {
                LOG_ERROR << "Failed to create 2D texture";
                return nullptr;
            }

            for (uint32_t level = 1u; level < levelsCount; ++level)
                result->setSubImage(level,
                    glm::uvec3(0u),
                    glm::uvec3(layerData[level]->width(), layerData[level]->height(), 0u),
                    numComponents,
                    type,
                    layerData[level]->data());
            break;
        }
        case graphics::TextureType::Type3D:
        {
            std::vector<std::shared_ptr<const utils::Image>> layersData(layersCount);
            for (uint32_t layer = 0u; layer < layersCount; ++layer)
                layersData[layer] = imagesData[layer][0u];

            result = graphicsRenderer->createTexture3D(layersData,
                textureInternalFormat,
                levelsCount,
                textureAutogenMipmaps);
            if (!result)
            {
                LOG_ERROR << "Failed to create 3D texture";
                return nullptr;
            }

            for (uint32_t layer = 0u; layer < layersCount; ++layer)
                for (uint32_t level = 1u; level < levelsCount; ++level)
                    result->setSubImage(level,
                        glm::uvec3(0u, 0u, layer),
                        glm::uvec3(imagesData[layer][level]->width(), imagesData[layer][level]->height(), 1u),
                        numComponents,
                        type,
                        imagesData[layer][level]->data());
            break;
        }
        case graphics::TextureType::TypeCube:
        {
            // TODO: replace "layer" to "face", and "face" to "level" representation as it was before
            //       to be able to store mipmap levels for cube texture

            if (layersCount != 6u)
            {
                LOG_ERROR << "Images layers count of cube texture must be 6";
                return nullptr;
            }

            std::vector<std::shared_ptr<const utils::Image>> layersData(layersCount);
            for (uint32_t layer = 0u; layer < layersCount; ++layer)
                layersData[layer] = imagesData[layer][0u];

            result = graphicsRenderer->createTextureCube(layersData,
                textureInternalFormat,
                levelsCount,
                textureAutogenMipmaps);

            if (!result)
            {
                LOG_ERROR << "Failed to create rect texture";
                return nullptr;
            }

            for (uint32_t layer = 0u; layer < layersCount; ++layer)
                for (uint32_t level = 1u; level < levelsCount; ++level)
                    result->setSubImage(level,
                        glm::uvec3(0u, 0u, layer),
                        glm::uvec3(imagesData[layer][level]->width(), imagesData[layer][level]->height(), 1u),
                        numComponents,
                        type,
                        imagesData[layer][level]->data());

            break;
        }
        case graphics::TextureType::Type1DArray:
        {
            std::vector<std::shared_ptr<const utils::Image>> layersData(layersCount);
            for (uint32_t layer = 0u; layer < layersCount; ++layer)
                layersData[layer] = imagesData[layer][0u];

            result = graphicsRenderer->createTexture1DArray(layersData,
                textureInternalFormat,
                levelsCount,
                textureAutogenMipmaps);
            if (!result)
            {
                LOG_ERROR << "Failed to create 1D array texture";
                return nullptr;
            }

            for (uint32_t layer = 0u; layer < layersCount; ++layer)
                for (uint32_t level = 1u; level < levelsCount; ++level)
                    result->setSubImage(level,
                        glm::uvec3(0u, layer, 0u),
                        glm::uvec3(imagesData[layer][level]->width(), 1u, 0u),
                        numComponents,
                        type,
                        imagesData[layer][level]->data());
            break;
        }
        case graphics::TextureType::Type2DArray:
        {
            std::vector<std::shared_ptr<const utils::Image>> layersData(layersCount);
            for (uint32_t layer = 0u; layer < layersCount; ++layer)
                layersData[layer] = imagesData[layer][0u];

            result = graphicsRenderer->createTexture2DArray(layersData,
                textureInternalFormat,
                levelsCount,
                textureAutogenMipmaps);
            if (!result)
            {
                LOG_ERROR << "Failed to create 2D array texture";
                return nullptr;
            }

            for (uint32_t layer = 0u; layer < layersCount; ++layer)
                for (uint32_t level = 1u; level < levelsCount; ++level)
                    result->setSubImage(level,
                        glm::uvec3(0u, 0u, layer),
                        glm::uvec3(imagesData[layer][level]->width(), imagesData[layer][level]->height(), 1u),
                        numComponents,
                        type,
                        imagesData[layer][level]->data());
            break;
        }
        case graphics::TextureType::TypeCubeArray:
        {
            if (levelsCount != 6u)
            {
                LOG_ERROR << "Images levels count of cube array texture must be 6";
                return nullptr;
            }

            result = graphicsRenderer->createTextureCubeArray(imagesData,
                textureInternalFormat,
                levelsCount,
                textureAutogenMipmaps);
            if (!result)
            {
                LOG_ERROR << "Failed to create rect array texture";
                return nullptr;
            }

            break;
        }
        case graphics::TextureType::TypeRect:
        {
            if (layersCount != 1u)
            {
                LOG_ERROR << "Images layers count of rect texture must be 1";
                return nullptr;
            }

            if (levelsCount != 1u)
            {
                LOG_ERROR << "Images levels count of rect texture must be 1";
                return nullptr;
            }

            result = graphicsRenderer->createTextureRect(imagesData[0u][0u], textureInternalFormat);
            if (!result)
            {
                LOG_ERROR << "Failed to create rect texture";
                return nullptr;
            }
            break;
        }
        default:
            break;
        }

        if (textureWrapMode.has_value())
            result->setWrapMode(textureWrapMode.value());

        if (textureFilterMode.has_value())
            result->setFilterMode(textureFilterMode.value());

        if (textureBorderColor.has_value())
            result->setBorderColor(textureBorderColor.value());

        return result;
    }

};

TexturesLoader::TexturesLoader(const std::string& name, const std::shared_ptr<graphics::RendererBase>& renderer)
    : ResourceLoaderBase<graphics::ITexture>(name)
    , m_(std::make_unique<TexturesLoaderPrivate>(renderer))
{
    registerLoader(std::make_shared<StandrdTexturesLoader>(name + "StandardLoader", renderer));
    registerLoader(std::make_shared<JSONTexturesLoader>(name + "JSONLoader", renderer));
}

TexturesLoader::~TexturesLoader() = default;

std::shared_ptr<graphics::ITexture> TexturesLoader::loadOrGet(const std::shared_ptr<const utils::Image>& image)
{
    if (!image)
    {
        LOG_ERROR << "Image can't be nullptr";
        return nullptr;
    }

    const std::string name = std::to_string(reinterpret_cast<uintptr_t>(image.get()));

    if (auto texture = findResource(name))
        return texture;

    auto texture = m_->renderer()->createTexture2D(image);
    if (!texture)
    {
        LOG_ERROR << "Failed to create texture";
        return nullptr;
    }

    addResource(name, texture);
    return texture;
}

std::shared_ptr<graphics::ITexture> TexturesLoader::loadOrGetDefaultIBLEnvironmentTexture()
{
    return loadOrGet(resources::DefaultIBLEnvironmentMapPath);
}

std::shared_ptr<graphics::ITexture> TexturesLoader::loadOrGetDefaultIBLBRDFLutTexture()
{
    return loadOrGet(resources::DefaultIBLBRDFLutMapPath);
}

std::shared_ptr<graphics::ITexture> TexturesLoader::loadOrGetDefaultIBLDiffuseTexture()
{
    return loadOrGet(resources::DefaultIBLDiffuseMapPath);
}

std::shared_ptr<graphics::ITexture> TexturesLoader::loadOrGetDefaultIBLSpecularTexture()
{
    return loadOrGet(resources::DefaultIBLSpecularMapPath);
}

}
}
