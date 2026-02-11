#ifndef APPLICATIONBASE_H
#define APPLICATIONBASE_H

#include <memory>
#include <filesystem>

#include <utils/noncopyble.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>
#include <core/inamedobject.h>

namespace simplex
{
namespace core
{

class ApplicationBasePrivate;
class CORE_SHARED_EXPORT ApplicationBase : public INamedObject
{
    NONCOPYBLE(ApplicationBase)

public:
    ~ApplicationBase() override;

    const std::string &name() const override;

    bool registerDevice(const std::shared_ptr<IDevice>&);
    bool unregisterDevice(const std::shared_ptr<IDevice>&);
    bool isDeviceRegistered(const std::shared_ptr<IDevice>&);

    void run();
    void stop();

    static bool initialize(ApplicationTimeCallback, ApplicationPollEventsCallback);
    static ApplicationBase& instance();

    void shutDown();
    //void update(uint64_t time, uint32_t dt);

    std::shared_ptr<Scene> scene();
    std::shared_ptr<const Scene> scene() const;
    void setScene(const std::shared_ptr<Scene>&);

protected:
    ApplicationBase(const std::string&, ApplicationTimeCallback, ApplicationPollEventsCallback);

    std::unique_ptr<ApplicationBasePrivate> m_;

};

}
}

#endif // APPLICATIONBASE_H
