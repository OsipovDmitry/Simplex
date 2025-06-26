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
class CORE_SHARED_EXPORT ApplicationBase : public std::enable_shared_from_this<ApplicationBase>, public INamedObject
{
    NONCOPYBLE(ApplicationBase)

public:
    ApplicationBase(const std::string&);
    ~ApplicationBase() override;

    const std::string &name() const override;

    static std::shared_ptr<ApplicationBase> currentApplication();

    void shutDown();
    void update(const std::shared_ptr<IRenderWidget>&, uint64_t time, uint32_t dt);

    const std::unordered_set<std::shared_ptr<IEngine>> &engines() const;
    bool registerEngine(std::shared_ptr<IEngine>);
    bool unregisterEngine(std::shared_ptr<IEngine>);

    std::shared_ptr<IEngine> findEngine(const std::string&);
    template <typename TEngine> std::shared_ptr<TEngine> findEngine(const std::string& = "");

    const std::unordered_set<std::shared_ptr<Scene>> &scenes() const;
    void removeScene(const std::shared_ptr<Scene>&);
    std::shared_ptr<Scene> loadEmptyScene(const std::string&);
    std::shared_ptr<Scene> loadGLTFScene(const std::filesystem::path&, const std::shared_ptr<graphics::IFrameBuffer>&);

protected:
    virtual void doInitialize();
    virtual void doUpdate(uint64_t time, uint32_t dt);

private:
    std::unique_ptr<ApplicationBasePrivate> m_;

};

template<typename TEngine>
inline std::shared_ptr<TEngine> ApplicationBase::findEngine(const std::string &engineName)
{
    std::shared_ptr<TEngine> result;
    for (const auto &engine : engines())
        if (auto castedEngine = std::dynamic_pointer_cast<TEngine>(engine);
            castedEngine && (engineName.empty() || castedEngine->name() == engineName))
        {
            result = castedEngine;
            break;
        }
    return result;
}

}
}

#endif // APPLICATIONBASE_H
