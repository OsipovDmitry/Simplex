#ifndef APPLICATIONBASEPRIVATE_H
#define APPLICATIONBASEPRIVATE_H

#include <set>
#include <memory>

#include <core/forwarddecl.h>

class ApplicationBasePrivate
{
public:
    ApplicationBasePrivate() {}

    std::set<std::shared_ptr<IEngine>> controllers;
};

#endif // APPLICATIONBASEPRIVATE_H
