#include "applicationbaseprivate.h"

namespace simplex
{
namespace core
{

std::weak_ptr<ApplicationBase> ApplicationBasePrivate::s_currentApplication;

ApplicationBasePrivate::ApplicationBasePrivate(const std::string &name)
    : m_name(name)
    , m_isInitialized(false)
{}

std::string &ApplicationBasePrivate::name()
{
    return m_name;
}

bool &ApplicationBasePrivate::isInitialized()
{
    return m_isInitialized;
}

debug::Information &ApplicationBasePrivate::debugInformation()
{
    return m_debugInformation;
}

std::unordered_set<std::shared_ptr<IEngine>> &ApplicationBasePrivate::engines()
{
    return m_engines;
}

std::unordered_set<std::shared_ptr<Scene>> &ApplicationBasePrivate::scenes()
{
    return m_scenes;
}

std::weak_ptr<ApplicationBase> &ApplicationBasePrivate::currentApplication()
{
    return s_currentApplication;
}

}
}
