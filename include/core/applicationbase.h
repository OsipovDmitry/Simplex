#ifndef APPLICATIONBASE_H
#define APPLICATIONBASE_H

#include <memory>

#include <utils/pimpl.h>
#include <utils/noncopyble.h>
#include <core/forwarddecl.h>
#include <core/iapplication.h>

class ApplicationBasePrivate;
class ApplicationBase : public IApplication
{
    PIMPL(ApplicationBase)
    NONCOPYBLE(ApplicationBase)

public:
    ApplicationBase();

    bool registerEngine(std::shared_ptr<IEngine>);
    bool unregisterEngine(std::shared_ptr<IEngine>);

    void update() override final;

protected:
    virtual void doUpdate();

private:
    std::unique_ptr<ApplicationBasePrivate> m_;

};

#endif // APPLICATIONBASE_H
