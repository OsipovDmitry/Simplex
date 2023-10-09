#include <iostream>

#include <utils/logger.h>
#include <utils/textfile.h>
#include <utils/image.h>
#include <utils/cubemap.h>
#include <utils/iblcalculator.h>

class ConsoleLoggerOutput : public simplex::utils::Logger::Output
{
public:
    ConsoleLoggerOutput() : simplex::utils::Logger::Output() {}
    virtual ~ConsoleLoggerOutput() override = default;
    void operator <<(const std::string &value) override { std::cout << value << std::endl; }
};

const auto readUint = [](const std::string &s, uint32_t &res, const std::string &arg)
{
    auto result = true;
    try
    {
        const int i{std::stoi(s)};
        if (i <= 0)
        {
            LOG_ERROR << arg << " must be greater than 0";
            result = false;
        }
        res = static_cast<uint32_t>(i);
    }
    catch (std::invalid_argument const& ex)
    {
        LOG_ERROR << arg << "is invalid: " << ex.what();
        result = false;
    }
    catch (std::out_of_range const& ex)
    {
        LOG_ERROR << arg << "is out of range: " << ex.what();
        result = false;
    }

    return result;
};

const auto readFloat = [](const std::string &s, float &res, const std::string &arg)
{
    auto result = true;
    try
    {
        const float f{std::stof(s)};
        if (f <= 0.f)
        {
            LOG_ERROR << arg << " must be greater than 0";
            result = false;
        }
        res = f;
    }
    catch (std::invalid_argument const& ex)
    {
        LOG_ERROR << arg << "is invalid: " << ex.what();
        result = false;
    }
    catch (std::out_of_range const& ex)
    {
        LOG_ERROR << arg << "is out of range: " << ex.what();
        result = false;
    }

    return result;
};

std::string duration2String(const std::chrono::steady_clock::time_point& begin, const std::chrono::steady_clock::time_point &end)
{
    std::string result;

    std::chrono::milliseconds::rep ms = 0, s = 0, m = 0;
    ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    s = ms / 1000;
    ms = ms % 1000;
    m = s / 60;
    s = s % 60;

    result = "(";
    if (m) result += std::to_string(m) + "m, ";
    if (s) result += std::to_string(s) + "s, ";
    result += std::to_string(ms) + "ms)";

    return result;
}

int main(int argc, char *argv[])
{
    std::chrono::steady_clock::time_point startTime;
    simplex::utils::Logger::setOutput(std::make_shared<ConsoleLoggerOutput>());

    std::filesystem::path environmentPath;
    std::filesystem::path outputDir = std::filesystem::current_path();
    bool noEnvironment = false;
    bool noBRDFLut = false;
    bool noIrradiance = false;
    bool noRadiance = false;
    uint32_t BRDFLutSize = 0u;
    uint32_t BRDFLutSamplesCount = 0u;
    uint32_t irradianceSize = 0;
    float irradianceSapmlesDeltaAngle = -1.f;
    uint32_t radianceSize = 0u;
    uint32_t radianceSamplesCount = 0u;
    uint32_t radianceLevelsCount = 0u;

    if (argc < 2)
    {
        LOG_ERROR << "Input environment map is not set";
        return 1;
    }
    else
        environmentPath = std::filesystem::absolute(argv[1]);

    for (int i = 2; i < argc; ++i)
    {
        const std::string arg(argv[i]);
        if (arg.rfind("-outdir=", 0) == 0 || arg.rfind("-output=", 0) == 0 || arg.rfind("-outputdir=", 0) == 0)
            outputDir = std::filesystem::absolute(arg.substr(arg.find("=")+1));
        else if (arg == "-no_env" || arg == "-no_environment")
            noEnvironment = true;
        else if (arg == "-no_brdf" || arg == "-no_brdflut")
            noBRDFLut = true;
        else if (arg == "-no_irrad" || arg == "-no_irradaince")
            noIrradiance = true;
        else if (arg == "-no_rad" || arg == "-no_radiance")
            noRadiance = true;
        else if (arg.rfind("-brdfsize=", 0) == 0 || arg.rfind("-brdflutsize=", 0) == 0)
        {
            const std::string argType = "BRDF Lut size";
            if (!readUint(arg.substr(arg.find("=")+1), BRDFLutSize, argType))
            {
                LOG_ERROR << "Failed to parse " << argType;
                return 1;
            }
        }
        else if (arg.rfind("-brdfsamples=", 0) == 0 || arg.rfind("-brdflutsamples=", 0) == 0)
        {
            const std::string argType = "BRDF Lut samples count";
            if (!readUint(arg.substr(arg.find("=")+1), BRDFLutSamplesCount, argType))
            {
                LOG_ERROR << "Failed to parse " << argType;
                return 1;
            }
        }
        else if (arg.rfind("-irradsize=", 0) == 0 || arg.rfind("-irradiancesize=", 0) == 0)
        {
            const std::string argType = "Irradiance size";
            if (!readUint(arg.substr(arg.find("=")+1), irradianceSize, argType))
            {
                LOG_ERROR << "Failed to parse " << argType;
                return 1;
            }
        }
        else if (arg.rfind("-irradangle=", 0) == 0 || arg.rfind("-irradianceangle=", 0) == 0)
        {
            const std::string argType = "Irradiance samples delta angle";
            if (!readFloat(arg.substr(arg.find("=")+1), irradianceSapmlesDeltaAngle, argType))
            {
                LOG_ERROR << "Failed to parse " << argType;
                return 1;
            }
        }
        else if (arg.rfind("-radsize=", 0) == 0 || arg.rfind("-radiancesize=", 0) == 0)
        {
            const std::string argType = "Radiance size";
            if (!readUint(arg.substr(arg.find("=")+1), radianceSize, argType))
            {
                LOG_ERROR << "Failed to parse " << argType;
                return 1;
            }
        }
        else if (arg.rfind("-radsamples=", 0) == 0 || arg.rfind("-radiancesamples=", 0) == 0)
        {
            const std::string argType = "Radiance samples count";
            if (!readUint(arg.substr(arg.find("=")+1), radianceSamplesCount, argType))
            {
                LOG_ERROR << "Failed to parse " << argType;
                return 1;
            }
        }
        else if (arg.rfind("-radlevels=", 0) == 0 || arg.rfind("-radiancelevels=", 0) == 0)
        {
            const std::string argType = "Radiance levels count";
            if (!readUint(arg.substr(arg.find("=")+1), radianceLevelsCount, argType))
            {
                LOG_ERROR << "Failed to parse " << argType;
                return 1;
            }
        }
        else
            LOG_WARNING << "Undefined argument " << arg;
    }

    std::string startMessage = "IBL Generator started; Arguments {";
    startMessage += " Input: " + environmentPath.string() + ";";
    if (noEnvironment) startMessage += " noEnvironment;";
    if (noBRDFLut) startMessage += " noBRDFLut;";
    if (noIrradiance) startMessage += " noIrradiance;";
    if (noRadiance) startMessage += " noRadiance;";
    if (BRDFLutSize) startMessage += " BRDFLutSize=" + std::to_string(BRDFLutSize) + ";";
    if (BRDFLutSamplesCount) startMessage += " BRDFLutSamplesCount=" + std::to_string(BRDFLutSamplesCount) + ";";
    if (irradianceSize) startMessage += " IrradianceSize=" + std::to_string(irradianceSize) + ";";
    if (irradianceSapmlesDeltaAngle > 0.f) startMessage += " IrradianceSamplesDelatAngle=" + std::to_string(irradianceSapmlesDeltaAngle) + ";";
    if (radianceSize) startMessage += " RadianceSize=" + std::to_string(radianceSize) + ";";
    if (radianceSamplesCount) startMessage += " RadianceSamplesCount=" + std::to_string(radianceSamplesCount) + ";";
    if (radianceLevelsCount) startMessage += " RadianceLevelsCount=" + std::to_string(radianceLevelsCount) + ";";
    startMessage.resize(startMessage.size() - 1);
    startMessage += "}";
    LOG_INFO << startMessage;

    auto sourceImage = simplex::utils::Image::loadFromFile(environmentPath);
    if (!sourceImage)
    {
        LOG_ERROR << "Failed to load source image " << environmentPath;
        return 1;
    }

    std::vector<std::shared_ptr<simplex::utils::Image>> environmentImages;
    if (!noEnvironment)
    {
        startTime = std::chrono::steady_clock::now();
        LOG_INFO << "Started to convert to environment faces list";
        environmentImages = simplex::utils::CubemapConverter::convertSphericalCubemapToFacesList(sourceImage);
        if (environmentImages.empty())
        {
            LOG_ERROR << "Failed to convert to environment faces list";
            return 1;
        }
        LOG_INFO << "Finished to convert to environment faces list " << duration2String(startTime, std::chrono::steady_clock::now());
    }

    std::shared_ptr<simplex::utils::Image> BRDFLutImage;
    if (!noBRDFLut)
    {
        if (BRDFLutSize) simplex::utils::IBLCalculator::setBRDFLutSize(BRDFLutSize);
        if (BRDFLutSamplesCount) simplex::utils::IBLCalculator::setBRDFLutSamplesCount(BRDFLutSamplesCount);

        startTime = std::chrono::steady_clock::now();
        LOG_INFO << "Started to calculate BRDF Lut";
        BRDFLutImage = simplex::utils::IBLCalculator::calclulateBRDFLut();
        if (!BRDFLutImage)
        {
            LOG_ERROR << "Failed to calculate BRDF Lut";
            return 1;
        }
        LOG_INFO << "Finished to calculate BRDF Lut " << duration2String(startTime, std::chrono::steady_clock::now());
    }

    std::vector<std::shared_ptr<simplex::utils::Image>> irradianceImages;
    if (!noIrradiance)
    {
        if (irradianceSize) simplex::utils::IBLCalculator::setIrradianseSize(irradianceSize);
        if (irradianceSapmlesDeltaAngle > 0.f) simplex::utils::IBLCalculator::setIrradianceSapmlesDeltaAngle(irradianceSapmlesDeltaAngle);

        startTime = std::chrono::steady_clock::now();
        LOG_INFO << "Started to calculate irradiance";
        auto irradianceImage = simplex::utils::IBLCalculator::calculateIrradiance(sourceImage);
        if (!irradianceImage)
        {
            LOG_ERROR << "Failed to calculate irradiance";
            return 1;
        }
        LOG_INFO << "Finished to calculate irradiance " << duration2String(startTime, std::chrono::steady_clock::now());

        startTime = std::chrono::steady_clock::now();
        LOG_INFO << "Started to convert to irradiance faces list";
        irradianceImages = simplex::utils::CubemapConverter::convertSphericalCubemapToFacesList(irradianceImage);
        if (irradianceImages.empty())
        {
            LOG_ERROR << "Failed to convert to irradiance faces list";
            return 1;
        }
        LOG_INFO << "Finished to convert to irradiance faces list " << duration2String(startTime, std::chrono::steady_clock::now());
    }

    std::vector<std::vector<std::shared_ptr<simplex::utils::Image>>> radianceLevelsImages(6u);
    if (!noRadiance)
    {
        if (radianceSize) simplex::utils::IBLCalculator::setRadianceSize(radianceSize);
        if (radianceSamplesCount) simplex::utils::IBLCalculator::setRadianceSamplesCount(radianceSamplesCount);

        startTime = std::chrono::steady_clock::now();
        LOG_INFO << "Started to calculate radiance";
        auto radianceLevelsImage = simplex::utils::IBLCalculator::calculateRadiance(sourceImage, radianceLevelsCount);
        if (radianceLevelsImage.empty())
        {
            LOG_ERROR << "Failed to calculate radiance";
            return 1;
        }
        LOG_INFO << "Finished to calculate radiance " << duration2String(startTime, std::chrono::steady_clock::now());

        startTime = std::chrono::steady_clock::now();
        LOG_INFO << "Started to convert to radiance faces list";
        for (uint32_t level = 0u; level < radianceLevelsImage.size(); ++level)
        {
            auto radianceLevelFacesImages = simplex::utils::CubemapConverter::convertSphericalCubemapToFacesList(radianceLevelsImage[level]);
            if (radianceLevelFacesImages.size() != 6u)
            {
                LOG_ERROR << "Failed to convert to radiance faces list";
                return 1;
            }
            for (uint32_t face = 0; face < radianceLevelFacesImages.size(); ++face)
                radianceLevelsImages[face].emplace_back(std::move(radianceLevelFacesImages[face]));
        }
        LOG_INFO << "Finished to convert to radiance faces list " << duration2String(startTime, std::chrono::steady_clock::now());
    }

    if (!noEnvironment)
    {
        startTime = std::chrono::steady_clock::now();
        LOG_INFO << "Started to save environment images";
        for (uint32_t face = 0u; face < environmentImages.size(); ++face)
        {
            const std::filesystem::path filename = outputDir / ("environment/" + std::to_string(face) + ".hdr");
            std::filesystem::create_directories(filename.parent_path());
            environmentImages[face]->saveToFile(filename);
        }
        LOG_INFO << "Finished to save environment images " << duration2String(startTime, std::chrono::steady_clock::now());

        LOG_INFO << "Started to save environment description";
        std::string desc;
        desc += "{\n";
        desc += "\t\"Type\" : \"Cube\",\n";
        desc += "\t\"InternalFormat\" : \"RGB9_E5\",\n";
        desc += "\t\"Data\" : [";
        for (uint32_t face = 0u; face < environmentImages.size(); ++face)
            desc += "\"environment/" + std::to_string(face) + ".hdr\", ";
        desc.resize(desc.size()-2u);
        desc += "]\n";
        desc += "}";
        simplex::utils::TextFile::loadFromData(desc)->saveToFile(outputDir / "environment.json");
        LOG_INFO << "Finished to save environment description";
    }

    if (!noBRDFLut)
    {
        startTime = std::chrono::steady_clock::now();
        LOG_INFO << "Started to save BRDF Lut image";
        const std::filesystem::path filename = outputDir / ("brdf/brdf.png");
        std::filesystem::create_directories(filename.parent_path());
        BRDFLutImage->saveToFile(filename);
        LOG_INFO << "Finished to save BRDF Lut image " << duration2String(startTime, std::chrono::steady_clock::now());

        LOG_INFO << "Started to save BRDF Lut description";
        std::string desc;
        desc += "{\n";
        desc += "\t\"Type\" : \"2D\",\n";
        desc += "\t\"InternalFormat\" : \"RG8\",\n";
        desc += "\t\"Wrap\" : \"ClampToEdge\",\n";
        desc += "\t\"Data\" : \"brdf/brdf.png\"\n";
        desc += "}";
        simplex::utils::TextFile::loadFromData(desc)->saveToFile(outputDir / "brdf.json");
        LOG_INFO << "Finished to save BRDF Lut description";
    }

    if (!noIrradiance)
    {
        startTime = std::chrono::steady_clock::now();
        LOG_INFO << "Started to save irradiance images";
        for (uint32_t face = 0u; face < irradianceImages.size(); ++face)
        {
            const std::filesystem::path filename = outputDir / ("irradiance/" + std::to_string(face) + ".hdr");
            std::filesystem::create_directories(filename.parent_path());
            irradianceImages[face]->saveToFile(filename);
        }
        LOG_INFO << "Finished to save irradiance images " << duration2String(startTime, std::chrono::steady_clock::now());

        LOG_INFO << "Started to save irradiance description";
        std::string desc;
        desc += "{\n";
        desc += "\t\"Type\" : \"Cube\",\n";
        desc += "\t\"InternalFormat\" : \"RGB9_E5\",\n";
        desc += "\t\"Data\" : [";
        for (uint32_t face = 0u; face < irradianceImages.size(); ++face)
            desc += "\"irradiance/" + std::to_string(face) + ".hdr\", ";
        desc.resize(desc.size()-2u);
        desc += "]\n";
        desc += "}";
        simplex::utils::TextFile::loadFromData(desc)->saveToFile(outputDir / "irradiance.json");
        LOG_INFO << "Finished to save irradiance description";
    }

    if (!noRadiance)
    {
        startTime = std::chrono::steady_clock::now();
        LOG_INFO << "Started to save radiance images";
        for (uint32_t face = 0u; face < radianceLevelsImages.size(); ++face)
            for (uint32_t level = 0u; level < radianceLevelsImages[face].size(); ++level)
            {
                const std::filesystem::path filename = outputDir / ("radiance/" + std::to_string(face) + "_" + std::to_string(level) + ".hdr");
                std::filesystem::create_directories(filename.parent_path());
                radianceLevelsImages[face][level]->saveToFile(filename);
            }
        LOG_INFO << "Finished to save radiance images " << duration2String(startTime, std::chrono::steady_clock::now());

        LOG_INFO << "Started to save radiance description";
        std::string desc;
        desc += "{\n";
        desc += "\t\"Type\" : \"Cube\",\n";
        desc += "\t\"InternalFormat\" : \"RGB9_E5\",\n";
        desc += "\t\"Data\" : [\n";
        for (uint32_t face = 0u; face < radianceLevelsImages.size(); ++face)
        {
            desc += "\t\t[";
            for (uint32_t level = 0u; level < radianceLevelsImages[face].size(); ++level)
                desc += "\"radiance/" + std::to_string(face) + "_" + std::to_string(level) + ".hdr\", ";
            desc.resize(desc.size()-2u);
            desc += "],\n";
        }
        desc.resize(desc.size()-2u);
        desc += "]\n";
        desc += "}";
        simplex::utils::TextFile::loadFromData(desc)->saveToFile(outputDir / "radiance.json");
        LOG_INFO << "Finished to save radiance description";
    }

    LOG_INFO << "IBL Generator finished";
    return 0;
}
