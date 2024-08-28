#ifndef CORE_APPLICATIONBASEPRIVATE_H
#define CORE_APPLICATIONBASEPRIVATE_H

#include <memory>
#include <unordered_set>
#include <string>

#include <core/forwarddecl.h>
#include <core/debuginformation.h>

namespace simplex
{
namespace core
{

class ApplicationBasePrivate
{
public:
    ApplicationBasePrivate(const std::string&);

    std::string &name();
    bool &isInitialized();
    debug::Information &debugInformation();
    std::unordered_set<std::shared_ptr<IEngine>> &engines();
    std::unordered_set<std::shared_ptr<Scene>> &scenes();
    static std::weak_ptr<ApplicationBase> &currentApplication();

private:
    std::string m_name;
    bool m_isInitialized;
    debug::Information m_debugInformation;
    std::unordered_set<std::shared_ptr<IEngine>> m_engines;
    std::unordered_set<std::shared_ptr<Scene>> m_scenes;
    static std::weak_ptr<ApplicationBase> s_currentApplication;
};

}
}

#endif // CORE_APPLICATIONBASEPRIVATE_H
