#ifndef APPLICATIONBASEPRIVATE_H
#define APPLICATIONBASEPRIVATE_H

#include <unordered_map>
#include <memory>
#include <string>


#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class ApplicationBasePrivate
{
public:
    ApplicationBasePrivate(const std::string &name)
        : m_name(name)
    {}

    std::string &name() { return m_name; }
    std::unordered_map<std::string, std::shared_ptr<IEngine>> &engines() { return m_engines; }

private:
    std::string m_name;
    std::unordered_map<std::string, std::shared_ptr<IEngine>> m_engines;
};

}
}

#endif // APPLICATIONBASEPRIVATE_H
