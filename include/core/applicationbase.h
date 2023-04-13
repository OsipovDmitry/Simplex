#ifndef APPLICATIONBASE_H
#define APPLICATIONBASE_H

#include <memory>

#include <utils/pimpl.h>
#include <utils/noncopyble.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>
#include <core/iapplication.h>

namespace simplex
{
namespace core
{

class ApplicationBasePrivate;
class CORE_SHARED_EXPORT ApplicationBase : public IApplication
{
    PIMPL(ApplicationBase)
    NONCOPYBLE(ApplicationBase)

public:
    ApplicationBase(const std::string&);
    ~ApplicationBase() override;

    const std::string &name() const override;

    bool registerEngine(std::shared_ptr<IEngine>);
    bool unregisterEngine(std::shared_ptr<IEngine>);

    void update(uint64_t time, uint32_t dt) override final;

protected:
    virtual void doUpdate(uint64_t, uint32_t) {}

private:
    std::unique_ptr<ApplicationBasePrivate> m_;

};

}
}

#endif // APPLICATIONBASE_H
