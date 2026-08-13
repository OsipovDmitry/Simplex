#ifndef CORE_BLOOM_H
#define CORE_BLOOM_H

#include <memory>

#include <utils/noncopyble.h>

#include <core/coreglobal.h>

namespace simplex
{
namespace core
{

class BloomPrivate;
class CORE_SHARED_EXPORT Bloom
{
    NONCOPYBLE(Bloom)
public:
    Bloom(std::unique_ptr<BloomPrivate>&&);
    ~Bloom();

    bool isEnabled() const;
    void setEnabled(bool);

    float contribution() const;
    void setContribution(float);

    uint32_t passesCount() const;
    void setPassesCount(uint32_t);

private:
    std::unique_ptr<BloomPrivate> m_;
};

} // namespace core
} // namespace simplex

#endif // CORE_BLOOM_H
