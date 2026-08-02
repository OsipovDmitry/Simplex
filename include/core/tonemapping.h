#ifndef CORE_TONEMAPPING_H
#define CORE_TONEMAPPING_H

#include <memory>

#include <utils/forwarddecl.h>
#include <utils/noncopyble.h>

#include <core/coreglobal.h>

namespace simplex
{
namespace core
{

class ToneMappingPrivate;
class CORE_SHARED_EXPORT ToneMapping
{
    NONCOPYBLE(ToneMapping)
public:
    ToneMapping(std::unique_ptr<ToneMappingPrivate>&&);
    ~ToneMapping();

    const utils::Range& luminanceRange() const;
    void setLuminanceRange(const utils::Range&);

    const utils::Range& luminanceClampRange() const;
    void setLuminanceClampRange(const utils::Range&);

    const std::pair<float, float>& pixelsFractionToTrim() const;
    void setPixelsFractionToTrim(const std::pair<float, float>&);

    float tauLight() const;
    void setTauLight(float);

    float tauDark() const;
    void setTauDark(float);

    float baseLuminance() const;
    void setBaseLuminance(float);

private:
    std::unique_ptr<ToneMappingPrivate> m_;
};

} // namespace core
} // namespace simplex

#endif // CORE_TONEMAPPING_H
