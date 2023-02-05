#ifndef APPLICATIONBASEPRIVATE_H
#define APPLICATIONBASEPRIVATE_H

#include <set>
#include <memory>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class ApplicationBasePrivate
{
public:
    ApplicationBasePrivate() {}

    std::set<std::shared_ptr<IEngine>> engines;
};

}
}

#endif // APPLICATIONBASEPRIVATE_H
